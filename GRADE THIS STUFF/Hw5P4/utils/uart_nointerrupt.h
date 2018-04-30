//*****************************************************************************
//
// UART_NOINTERRUPT.h - Prototypes for the UART console functions.
//
// Copyright (c) 2007-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.0.12573 of the Tiva Utility Library.
//
//*****************************************************************************

#ifndef __UART_NOINTERRUPT_H__
#define __UART_NOINTERRUPT_H__

#include <stdint.h>

int8_t initUART(uint8_t portNum, uint32_t clock, uint32_t sysclock, uint32_t parity);

int8_t sendUARTline(uint8_t portNum, char* str);

int8_t getUARTline(uint8_t portNum, char* str, uint32_t maxlen);

int8_t wait_for_char_then_pull_UART(uint8_t portNum, char token, char* str, uint32_t len);

int8_t getUARTstring_block(uint8_t portNum, char* str, uint32_t len);

int8_t getUARTstring_nonblock(uint8_t portNum, char* str, uint32_t len);

int8_t sendUARTstring(uint8_t portNum, char* str, uint32_t len);

int8_t disableUART(uint8_t portNum);

#endif // __UART_NOINTERRUPT_H__
