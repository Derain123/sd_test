// See LICENSE for license details.

#ifndef SPI_HEADER_H
#define SPI_HEADER_H

#include <stdint.h>
#include "platform.h"

// SiFive SPI控制器基地址
#define SPI_BASE ((uint32_t)SPI_CTRL_ADDR)

/////////////////////////////
// SPI APIs

// 初始化SPI控制器
void spi_init();

// 禁用SPI控制器
void spi_disable();

// 发送一个字节并接收一个字节
uint8_t spi_send(uint8_t dat);

// 发送多个字节，n<=16
void spi_send_multi(const uint8_t* dat, uint8_t n);

// 接收多个字节，n<=16
void spi_recv_multi(uint8_t* dat, uint8_t n);

// 选择从机设备
void spi_select_slave(uint8_t id);

// 取消选择从机设备
void spi_deselect_slave(uint8_t id);

#endif
