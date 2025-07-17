// See LICENSE for license details.

#include <stdint.h>
#include "platform.h"
#include "devices/spi.h"

#define GetBit(r, p) (((r) & (1 <<p)) >> p)

volatile uint32_t *spi_base_ptr = (uint32_t *)(SPI_CTRL_ADDR);

void spi_init() {
  // 设置时钟分频
  SPI_REG(SPI_REG_SCKDIV) = 0x3; // 分频值，根据需要调整
  
  // 设置时钟模式 - 标准SPI模式
  SPI_REG(SPI_REG_SCKMODE) = 0x0;
  
  // 设置片选模式为自动
  SPI_REG(SPI_REG_CSMODE) = SPI_CSMODE_AUTO;
  
  // 设置格式寄存器 - 8位数据，MSB优先，标准SPI协议
  SPI_REG(SPI_REG_FMT) = SPI_FMT_PROTO(SPI_PROTO_S) | 
                          SPI_FMT_ENDIAN(SPI_ENDIAN_MSB) |
                          SPI_FMT_LEN(8);
  
  // 设置发送和接收FIFO水位
  SPI_REG(SPI_REG_TXCTRL) = SPI_TXWM(1);
  SPI_REG(SPI_REG_RXCTRL) = SPI_RXWM(1);
  
  // 使能SPI控制器
  SPI_REG(SPI_REG_FCTRL) = SPI_FCTRL_EN;
}

void spi_disable() {
  // 禁用SPI控制器
  SPI_REG(SPI_REG_FCTRL) = 0x0;
}

uint8_t spi_send(uint8_t dat) {
  int32_t r;
  
  // 发送数据
  SPI_REG(SPI_REG_TXFIFO) = dat;
  
  // 等待接收数据
  do {
    r = SPI_REG(SPI_REG_RXFIFO);
  } while (r < 0);
  
  return (uint8_t)r;
}

void spi_send_multi(const uint8_t* dat, uint8_t n) {
  uint8_t i;
  
  // 发送多个字节
  for(i = 0; i < n; i++) {
    SPI_REG(SPI_REG_TXFIFO) = *(dat++);
  }
  
  // 等待所有数据发送完成
  while(!GetBit(SPI_REG(SPI_REG_TXCTRL), 31));
  
  // 清空接收FIFO
  for(i = 0; i < n; i++) {
    volatile int32_t r;
    do {
      r = SPI_REG(SPI_REG_RXFIFO);
    } while (r < 0);
  }
}

void spi_recv_multi(uint8_t* dat, uint8_t n) {
  uint8_t i;
  
  // 发送空字节来接收数据
  for(i = 0; i < n; i++) {
    SPI_REG(SPI_REG_TXFIFO) = 0xFF;
  }
  
  // 等待所有数据发送完成
  while(!GetBit(SPI_REG(SPI_REG_TXCTRL), 31));
  
  // 接收数据
  for(i = 0; i < n; i++) {
    int32_t r;
    do {
      r = SPI_REG(SPI_REG_RXFIFO);
    } while (r < 0);
    *(dat++) = (uint8_t)r;
  }
}

void spi_select_slave(uint8_t id) {
  // 设置片选ID
  SPI_REG(SPI_REG_CSID) = id;
  
  // 切换到手动片选模式
  SPI_REG(SPI_REG_CSMODE) = SPI_CSMODE_HOLD;
}

void spi_deselect_slave(uint8_t id) {
  // 切换回自动片选模式
  SPI_REG(SPI_REG_CSMODE) = SPI_CSMODE_AUTO;
}

