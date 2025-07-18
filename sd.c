// See LICENSE.Sifive for license details.
#include <stdint.h>
#include <stddef.h>

#include <platform.h>

#include "common.h"
#include "diskio.h"
#include "ff.h"

#define DEBUG
#include "kprintf.h"

// Total payload in B
#define PAYLOAD_SIZE_B (30 << 20) // default: 30MiB
// A sector is 512 bytes, so (1 << 11) * 512B = 1 MiB
#define SECTOR_SIZE_B 512
// Payload size in # of sectors
#define PAYLOAD_SIZE (PAYLOAD_SIZE_B / SECTOR_SIZE_B)

// The sector at which the BBL partition starts
#define BBL_PARTITION_START_SECTOR 34

// File system read chunk size
#define FS_READ_SIZE 4096

// FAT filesystem work area
FATFS FatFs;

#ifndef TL_CLK
#error Must define TL_CLK
#endif

#define F_CLK 		(TL_CLK)

// SPI SCLK frequency, in kHz
// We are using the 25MHz High Speed mode. If this speed is not supported by the
// SD card, consider changing to the Default Speed mode (12.5 MHz).
#define SPI_CLK 	25000

// SPI clock divisor value
// @see https://ucb-bar.gitbook.io/baremetal-ide/baremetal-ide/using-peripheral-devices/sifive-ips/serial-peripheral-interface-spi
#define SPI_DIV 	(((F_CLK * 1000) / SPI_CLK) / 2 - 1)

static volatile uint32_t * const spi = (void *)(SPI_CTRL_ADDR);

static inline uint8_t spi_xfer(uint8_t d)
{
	int32_t r;

	REG32(spi, SPI_REG_TXFIFO) = d;
	do {
		r = REG32(spi, SPI_REG_RXFIFO);
	} while (r < 0);
	return r;
}

static inline uint8_t sd_dummy(void)
{
	return spi_xfer(0xFF);
}

static uint8_t sd_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	unsigned long n;
	uint8_t r;

	REG32(spi, SPI_REG_CSMODE) = SPI_CSMODE_HOLD;
	sd_dummy();
	spi_xfer(cmd);
	spi_xfer(arg >> 24);
	spi_xfer(arg >> 16);
	spi_xfer(arg >> 8);
	spi_xfer(arg);
	spi_xfer(crc);

	n = 1000;
	do {
		r = sd_dummy();
		if (!(r & 0x80)) {
//			dprintf("sd:cmd: %hx\r\n", r);
			goto done;
		}
	} while (--n > 0);
	kputs("sd_cmd: timeout");
done:
	return r;
}

static inline void sd_cmd_end(void)
{
	sd_dummy();
	REG32(spi, SPI_REG_CSMODE) = SPI_CSMODE_AUTO;
}


static void sd_poweron(void)
{
	long i;
	// HACK: frequency change

	REG32(spi, SPI_REG_SCKDIV) = SPI_DIV;
	REG32(spi, SPI_REG_CSMODE) = SPI_CSMODE_OFF;
	for (i = 10; i > 0; i--) {
		sd_dummy();
	}
	REG32(spi, SPI_REG_CSMODE) = SPI_CSMODE_AUTO;
}

static int sd_cmd0(void)
{
	int rc;
	dputs("CMD0");
	rc = (sd_cmd(0x40, 0, 0x95) != 0x01);
	sd_cmd_end();
	return rc;
}

static int sd_cmd8(void)
{
	int rc;
	dputs("CMD8");
	rc = (sd_cmd(0x48, 0x000001AA, 0x87) != 0x01);
	sd_dummy(); /* command version; reserved */
	sd_dummy(); /* reserved */
	rc |= ((sd_dummy() & 0xF) != 0x1); /* voltage */
	rc |= (sd_dummy() != 0xAA); /* check pattern */
	sd_cmd_end();
	return rc;
}

static void sd_cmd55(void)
{
	sd_cmd(0x77, 0, 0x65);
	sd_cmd_end();
}

static int sd_acmd41(void)
{
	uint8_t r;
	dputs("ACMD41");
	do {
		sd_cmd55();
		r = sd_cmd(0x69, 0x40000000, 0x77); /* HCS = 1 */
	} while (r == 0x01);
	return (r != 0x00);
}

static int sd_cmd58(void)
{
	int rc;
	dputs("CMD58");
	rc = (sd_cmd(0x7A, 0, 0xFD) != 0x00);
	rc |= ((sd_dummy() & 0x80) != 0x80); /* Power up status */
	sd_dummy();
	sd_dummy();
	sd_dummy();
	sd_cmd_end();
	return rc;
}

static int sd_cmd16(void)
{
	int rc;
	dputs("CMD16");
	rc = (sd_cmd(0x50, 0x200, 0x15) != 0x00);
	sd_cmd_end();
	return rc;
}

static uint16_t crc16_round(uint16_t crc, uint8_t data) {
	crc = (uint8_t)(crc >> 8) | (crc << 8);
	crc ^= data;
	crc ^= (uint8_t)(crc >> 4) & 0xf;
	crc ^= crc << 12;
	crc ^= (crc & 0xff) << 5;
	return crc;
}

#define SPIN_SHIFT	6
#define SPIN_UPDATE(i)	(!((i) & ((1 << SPIN_SHIFT)-1)))
#define SPIN_INDEX(i)	(((i) >> SPIN_SHIFT) & 0x3)

static const char spinner[] = { '-', '/', '|', '\\' };

static int copy(void)
{
	FIL fil;                // File object
	FRESULT fr;             // FatFs return code
	volatile uint8_t *p = (void *)(PAYLOAD_DEST);
	uint8_t *buf = (uint8_t *)p;
	uint32_t fsize = 0;     // file size count
	uint32_t br;            // Read count
	int rc = 0;

	kprintf("LOADING 0x%x B PAYLOAD FROM FILE\r\n", PAYLOAD_SIZE_B);
	kprintf("LOADING  ");

	// Mount the filesystem
	if(f_mount(&FatFs, "", 1)) {
		kputs("Fail to mount SD filesystem!\r\n");
		return 1;
	}
	kprintf("Mounted SD filesystem\r\n");

	// Open the payload file
	fr = f_open(&fil, "payload.bin", FA_READ);
	if (fr) {
		kputs("Failed to open payload.bin!\r\n");
		f_mount(NULL, "", 1); // unmount
		return (int)fr;
	}
	kprintf("Opened payload.bin\r\n");

	// Read file into memory
	uint32_t total_read = 0;
	do {
		fr = f_read(&fil, buf, FS_READ_SIZE, &br);  // Read a chunk of file
		buf += br;
		fsize += br;
		total_read += br;

		kprintf("Read %d bytes\r\n", br);
		
		// Update spinner
		if (SPIN_UPDATE(total_read >> 12)) { // Update every 4KB
			kputc('\b');
			kputc(spinner[SPIN_INDEX(total_read >> 12)]);
		}

		kprintf("Total read: %d bytes\r\n", total_read);
		
		// Limit to maximum payload size
		if (total_read >= PAYLOAD_SIZE_B) {
			break;
		}
	} while(!(fr || br == 0));

	kprintf("\b Loaded %d bytes from payload.bin\r\n", fsize);

	// Close the file
	if(f_close(&fil)) {
		kputs("fail to close file!\r\n");
		rc = 1;
	}
	
	// Unmount filesystem
	if(f_mount(NULL, "", 1)) {
		kputs("fail to umount filesystem!\r\n");
		rc = 1;
	}

	return rc;
}

int main(void)
{
	REG32(uart, UART_REG_TXCTRL) = UART_TXEN;

	kputs("INIT");
	sd_poweron();
	if (sd_cmd0() ||
	    sd_cmd8() ||
	    sd_acmd41() ||
	    sd_cmd58() ||
	    sd_cmd16()) {
		kputs("ERROR");
		return 1;
	}

	// copy payload.bin to SD card
	if (copy()) {
		kputs("ERROR");
		return 1;
	}

	kputs("BOOT");

	__asm__ __volatile__ ("fence.i" : : : "memory");

	return 0;
}
