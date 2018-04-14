//
/*****************************************************************************
 * Adapted from TI TivaWare User Guide, page 193 by Zach Farmer
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted by Texas Instruments as part of their product support
 * for the Tiva TMC1294XL Cortex M4F and other microcontrollers in
 * TI's ecosystem.
 *
*****************************************************************************/
/**
 * @file eth.c
 * @brief Ethernet MAC PHY HAL for the emac module on TMC1294XL Launchpad.
 * Requires TI driverlib.
 *
 * This is a short HAL driver for TI's emac module.
 *
 * @author Texas Instruments, modified by Zach Farmer
 * @date April 14 2018
 * @version 1.0
 *
 */

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
//
// Read a packet from the DMA receive buffer and return the number of bytes
// read.
//
//*****************************************************************************
int32_t ProcessReceivedPacket(void);

//*****************************************************************************
//
// The interrupt handler for the Ethernet interrupt.
//
//*****************************************************************************
void EthernetIntHandler(void);

//*****************************************************************************
//
// Transmit a packet from the supplied buffer. This function would be called
// directly by the application. pui8Buf points to the Ethernet frame to send
// and i32BufLen contains the number of bytes in the frame.
//
//*****************************************************************************
static int32_t PacketTransmit(uint8_t *pui8Buf, int32_t i32BufLen);

//*****************************************************************************
//
// Initialize the transmit and receive DMA descriptors.
//
//*****************************************************************************
void InitDescriptors(uint32_t ui32Base)

//*****************************************************************************
//
// Initialize the ethernet controller. Takes system clock speed.
//
//*****************************************************************************
uint32_t initEthernet(uint32_t ui32SysClock);