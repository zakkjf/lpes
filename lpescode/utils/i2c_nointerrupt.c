
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

static const uint32_t g_ui32I2CPeriph[8] =
{
    SYSCTL_PERIPH_I2C0,
    SYSCTL_PERIPH_I2C1,
    SYSCTL_PERIPH_I2C2,
    SYSCTL_PERIPH_I2C3,
    SYSCTL_PERIPH_I2C4,
    SYSCTL_PERIPH_I2C5,
    SYSCTL_PERIPH_I2C6,
    SYSCTL_PERIPH_I2C7,
    SYSCTL_PERIPH_I2C8,
    SYSCTL_PERIPH_I2C9

};

static const uint32_t g_ui32I2CBase[8] =
{
     I2C0_BASE,
     I2C1_BASE,
     I2C2_BASE,
     I2C3_BASE,
     I2C4_BASE,
     I2C5_BASE,
     I2C6_BASE,
     I2C7_BASE,
     I2C8_BASE,
     I2C9_BASE
};

uint8_t initi2c(uint32_t module, uint32_t sysclock)
{
    //
    // Enable the I2C0 peripheral
    //
    SysCtlPeripheralEnable(g_ui32I2CPeriph[module]);
    SysCtlPeripheralReset(g_ui32I2CPeriph[module]);
    //
    // Wait for the I2C0 module to be ready.
    //
    while(!SysCtlPeripheralReady(g_ui32I2CPeriph[module]))
    {
    }
    //
    // Initialize Master and Slave
    //

    I2CMasterEnable(g_ui32I2CBase[module]);
    //400kHz mode
    I2CMasterInitExpClk(g_ui32I2CBase[module] , sysclock, true);
    return 0;
}

uint8_t sendi2cbyte(uint32_t module, uint8_t address, uint8_t reg, uint8_t data)
{
    I2CMasterSlaveAddrSet(g_ui32I2CBase[module] , address, false);
    I2CMasterDataPut(g_ui32I2CBase[module] , reg);
    I2CMasterControl(g_ui32I2CBase[module] , I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(g_ui32I2CBase[module] ));

    I2CMasterDataPut(g_ui32I2CBase[module] , data);
    I2CMasterControl(g_ui32I2CBase[module] , I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(g_ui32I2CBase[module] ));
    return 0;
}

uint8_t geti2cbyte(uint32_t module, uint8_t address, uint8_t reg)
{
    uint32_t data;

    I2CMasterSlaveAddrSet(g_ui32I2CBase[module], address, false);
    I2CMasterDataPut(g_ui32I2CBase[module], reg);
    I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(g_ui32I2CBase[module]));

    I2CMasterSlaveAddrSet(g_ui32I2CBase[module], address, true);
    I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_SINGLE_RECEIVE);
    while(I2CMasterBusy(g_ui32I2CBase[module]));
    data = I2CMasterDataGet(g_ui32I2CBase[module]);

    return (uint8_t)(data & 0xFF);
}

/**
 * Send a byte-array of data through I2C bus(blocking)
 * @param module tiva module number 0-9 of the i2c interface
 * @param address 7-bit address of I2C device (8. bit is for R/W)
 * @param reg Address of register in I2C device to write into
 * @param data Data buffer of data to send
 * @param length Length of data to send
 * @return 0 so that caller can verify the function didn't hang
 */
uint8_t sendi2cbytes(uint8_t module, uint8_t address, uint8_t reg, uint16_t length, uint8_t *data)
{
    uint16_t i;
    I2CMasterSlaveAddrSet(g_ui32I2CBase[module], address, false);
    // send reg address
    I2CMasterDataPut(g_ui32I2CBase[module], reg);
    //burst mode
    I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(g_ui32I2CBase[module]));

    //  Loop through all data that needs to be sent. Send everything but last
    //  byte
    for (i = 0; i < (length-1); i++)
    {
        I2CMasterDataPut(g_ui32I2CBase[module], data[i]);
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_SEND_CONT);

        while(I2CMasterBusy(g_ui32I2CBase[module]));
    }

    //  Send last byte with stop sequence
    I2CMasterDataPut(g_ui32I2CBase[module], data[length-1]);
    I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_SEND_FINISH);

    while(I2CMasterBusy(g_ui32I2CBase[module]));

    return 0;
}


/**
 * Read several bytes from I2C device
 * @param module tiva module number 0-9 of the i2c interface
 * @param address 7-bit address of I2C device (8. bit is for R/W)
 * @param reg address of register in I2C device to write into
 * @param count number of bytes to red
 * @param dest pointer to data buffer in which data is saved after reading
 */
uint8_t geti2cbytes(uint8_t module, uint8_t address, uint8_t reg, uint16_t length, uint8_t* data)
{
    uint16_t i;

    I2CMasterSlaveAddrSet(g_ui32I2CBase[module], address, false);
    I2CMasterDataPut(g_ui32I2CBase[module], reg);
    I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(g_ui32I2CBase[module]));

    I2CMasterSlaveAddrSet(g_ui32I2CBase[module], address, true);

    if (length == 1)
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_SINGLE_RECEIVE);
    else
    {
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_RECEIVE_START);
        while(I2CMasterBusy(g_ui32I2CBase[module]));

        data[0] = (uint8_t)(I2CMasterDataGet(g_ui32I2CBase[module]) & 0xFF);
        for (i = 1; i < (length-1); i++)
        {
            I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            while(I2CMasterBusy(g_ui32I2CBase[module]));
            data[i] = (uint8_t)(I2CMasterDataGet(g_ui32I2CBase[module]) & 0xFF);
        }
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    }
    while(I2CMasterBusy(g_ui32I2CBase[module]));

    data[length-1] = (uint8_t)(I2CMasterDataGet(g_ui32I2CBase[module]) & 0xFF);
    return 0;
}

/**
 * Immediately fetch several bytes from I2C device, do not transmit addr or register
 * @param module tiva module number 0-9 of the i2c interface
 * @param count number of bytes to red
 * @param dest pointer to data buffer in which data is saved after reading
 */
uint8_t fetchi2cbytes(uint8_t module, uint8_t addr, uint8_t reg, uint16_t length, uint8_t* data)
{
    uint16_t i;

  /*  I2CMasterSlaveAddrSet(g_ui32I2CBase[module], addr1, false);
    I2CMasterDataPut(g_ui32I2CBase[module], reg);
    I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(g_ui32I2CBase[module]));
*/


    I2CMasterSlaveAddrSet(g_ui32I2CBase[module], addr, true);

    if (length == 1)
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_SINGLE_RECEIVE);
    else
    {
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_RECEIVE_START);
        while(I2CMasterBusy(g_ui32I2CBase[module]));

        data[0] = (uint8_t)(I2CMasterDataGet(g_ui32I2CBase[module]) & 0xFF);
        for (i = 1; i < (length-1); i++)
        {
            I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            while(I2CMasterBusy(g_ui32I2CBase[module]));
            data[i] = (uint8_t)(I2CMasterDataGet(g_ui32I2CBase[module]) & 0xFF);
        }
        I2CMasterControl(g_ui32I2CBase[module], I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    }
    while(I2CMasterBusy(g_ui32I2CBase[module]));

    data[length-1] = (uint8_t)(I2CMasterDataGet(g_ui32I2CBase[module]) & 0xFF);
    return 0;
}
