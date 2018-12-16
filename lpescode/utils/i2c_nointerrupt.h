/*
 * mpu9250.c
 *
 *  Created on: Apr 19, 2018
 *      Author: Adrestia
 */

#ifndef __I2C_NOINTERRUPT_H__
#define __I2C_NOINTERRUPT_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"

uint8_t initi2c(uint32_t module, uint32_t sysclock);

uint8_t sendi2cbyte(uint32_t module, uint8_t address, uint8_t reg, uint8_t data);

uint8_t geti2cbyte(uint32_t module, uint8_t address, uint8_t reg);

uint8_t sendi2cbytes(uint8_t module, uint8_t address, uint8_t reg, uint16_t length, uint8_t *data);

/**
 * Read several bytes from I2C device
 * @param module tiva module number 0-9 of the i2c interface
 * @param address 7-bit address of I2C device (8. bit is for R/W)
 * @param reg address of register in I2C device to write into
 * @param count number of bytes to red
 * @param dest pointer to data buffer in which data is saved after reading
 */
uint8_t geti2cbytes(uint8_t module, uint8_t address, uint8_t reg, uint16_t length, uint8_t* data);

uint8_t fetchi2cbytes(uint8_t module, uint8_t addr1, uint8_t addr2, uint8_t reg, uint16_t length, uint8_t* data);

#endif
