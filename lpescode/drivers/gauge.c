/*
 * This file implements drivers for the TI BQ35100 Gas Gauge IC
 *
 * The IC is an I2C interface part, the commands are hard to track down on
 * the internet but a not-very-concise reference can be found here:
 * http://www.ti.com/lit/ug/sluubh1c/sluubh1c.pdf
 *
 * These drivers are based on the code provided by TI in the Appendix of:
 * http://www.ti.com/lit/an/slua801/slua801.pdf
 * Arch-dependent functions read, write, and address are left to us to develop,
 * which we have done.
 *
 * References to void *pHandle (for arch-independence) are
 * removed in each function as we are only interested in one predetermined I2C
 * interface in our application. We replace these with i2c drivers we developed
 */

//#include "drivers/gauge.h"


#include "gauge.h"

#define SET_CFGUPDATE   0x0013
#define CMD_DATA_CLASS  0x3E
#define CMD_BLOCK_DATA  0x40
#define CMD_CHECK_SUM   0x60
#define CMD_FLAGS       0x06
#define CFGUPD          0x0010

//gauge_read: read bytes from gauge (must be implemented for a specific system)
//pHandle: handle to communications adapater (removed)
//nRegister: first register (=standard command) to read from
//pData: pointer to a data buffer
//nLength: number of bytes
//return value: number of bytes read (0 if error)
//int gauge_read(uint8_t cmd_addr, uint8_t cmd_reg, uint8_t read_addr,
//               uint8_t* read_cmd, uint8_t read_length, uint8_t* data)
int gauge_read(unsigned char nRegister, unsigned char *pData, unsigned char nLength)
{
    int n;

    if (nLength < 1)
        return 0;

    // per datasheet instructions, read sequence happens as:
    // send write command addr followed by little-endian write command reg
    // send write command addr, read command addr, read command reg,

   // pData[0] = nRegister;

  // sendI2Cbytes(I2C_GAUGE, GAUGE_I2C_ADDR, nRegister, 2, data);
   // n = write(pI2C->nI2C, pData, 1); // write register address
    //n = read(pI2C->nI2C, pData, nLength); // read data from register
 //   uint8_t geti2cbytes(uint8_t module, uint8_t address, uint8_t reg, uint16_t length, uint8_t* data);
 //  geti2cbytes(I2C_GAUGE, GAUGE_I2C_ADDR, nRegister, 1, pData);

  //  sendi2cbytes(I2C_GAUGE, GAUGE_I2C_ADDR_WRITE, nRegister, 2, pData);


    fetchi2cbytes(I2C_GAUGE, GAUGE_I2C_ADDR_WRITE, GAUGE_I2C_ADDR_READ, nRegister, nLength, pData);

    n = nLength;

    return n;
}

//gauge_write: write bytes to gauge (must be implemented for a specific system)
//pHandle: handle to communications adapater (removed)
//nRegister: first register (=standard command) to write to
//pData: pointer to a data buffer
//nLength: number of bytes
//return value: number of bytes written (0 if error)
int gauge_write(unsigned char nRegister, unsigned char *pData, unsigned char nLength)
{

    sendi2cbytes(I2C_GAUGE, GAUGE_I2C_ADDR_WRITE, nRegister, nLength, pData);

    return 0;
}


//gauge_control: issues a sub command
//pHandle: handle to communications adapter (removed)
//nSubCmd: sub command number
//return value: result from sub command
unsigned int gauge_control(unsigned int nSubCmd)
{
    unsigned int nResult = 0;
    char pData[2];

    pData[0] = nSubCmd & 0xFF;
    pData[1] = (nSubCmd >> 8) & 0xFF;

    gauge_write(0x00, pData, 2); // issue control and sub command
    gauge_read(0x00, pData, 2); // read data
    nResult = (pData[1] <<8) | pData[0];

    return nResult;
}

//gauge_cmd_read: read data from standard command
//pHandle: handle to communications adapter (removed
//nCmd: standard command
//return value: result from standard command
unsigned int gauge_cmd_read(unsigned char nCmd)
{
    unsigned char pData[2];

    gauge_read(nCmd, pData, 2);

    return (pData[1] << 8) | pData[0];
}

//gauge_cmd_write: write data to standard command
//pHandle: handle to communications adapter (removed)
//nCmd: standard command
//return value: number of bytes written to gauge
unsigned int gauge_cmd_write(unsigned char nCmd, unsigned int nData)
{
    unsigned char pData[2];

    pData[0] = nData & 0xFF;
    pData[1] = (nData >> 8) & 0xFF;

    return gauge_write(nCmd, pData, 2);
}

//gauge_cfg_update: enter configuration update mode for rom gauges
//pHandle: handle to communications adapter (removed)
//return value: true = success, false = failure
#define MAX_ATTEMPTS 5
bool gauge_cfg_update()
{
    unsigned int nFlags;
    int nAttempts = 0;
    unsigned int i = 0;

    gauge_control(SET_CFGUPDATE);

    do
    {
        nFlags = gauge_cmd_read(CMD_FLAGS);
        if (!(nFlags & CFGUPD))
        {
            for(i = 0; i < 65535; i++);
        }
    } while (!(nFlags & CFGUPD) && (nAttempts++ < MAX_ATTEMPTS));

    return (nAttempts < MAX_ATTEMPTS);
}

//gauge_exit: exit configuration update mode for rom gauges
//pHandle: handle to communications adapter (removed)
//nSubCmd: sub command to exit configuration update mode
//return value: true = success, false = failure
bool gauge_exit(unsigned int nSubCmd)
{
    unsigned int nFlags;
    int nAttempts = 0;
    unsigned int i = 0;

    gauge_control(nSubCmd);

    do
    {
        nFlags = gauge_cmd_read(CMD_FLAGS);
        if (nFlags & CFGUPD)
            {
                for(i = 0; i < 65535; i++);
            }
    } while ((nFlags & CFGUPD) && (nAttempts++ <MAX_ATTEMPTS));

    return (nAttempts < MAX_ATTEMPTS);
}

//gauge_read_data_class: read a data class
//pHandle: handle to communications adapter (removed)
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_read_data_class(unsigned char nDataClass, unsigned char *pData, unsigned char nLength)
{
    unsigned char nRemainder = nLength;
    unsigned int nOffset = 0;
    unsigned char nDataBlock = 0x00;
    unsigned int nData;

    if (nLength < 1)
        return 0;

    do
    {
        nLength = nRemainder;

        if (nLength > 32)
        {
            nRemainder = nLength - 32;
            nLength = 32;
        }

        else
            nRemainder = 0;

        nData = (nDataBlock << 8) | nDataClass;
        gauge_cmd_write(CMD_DATA_CLASS, nData);

        if (gauge_read(CMD_BLOCK_DATA, pData, nLength) != nLength)
            return -1;

        pData += nLength;
        nDataBlock++;

    } while (nRemainder > 0);

    return 0;
}

//check_sum: calculate check sum for block transfer
//pData: pointer to data block
//nLength: length of data block
unsigned char check_sum(unsigned char *pData, unsigned char nLength)
{
    unsigned char nSum = 0x00;
    unsigned char n;

    for (n = 0; n < nLength; n++)
        nSum += pData[n];

    nSum = 0xFF - nSum;

    return nSum;
}
//gauge_write_data_class: write a data class
//pHandle: handle to communications adapter (removed)
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_write_data_class(unsigned char nDataClass, unsigned char *pData,
                           unsigned char nLength)
{
    unsigned char nRemainder = nLength;
    unsigned int nOffset = 0;
    unsigned char pCheckSum[2] = {0x00, 0x00};
    unsigned int nData;
    unsigned char nDataBlock = 0x00;
    unsigned int i = 0;

    if (nLength < 1)
        return 0;

    do
    {
        nLength = nRemainder;
        if (nLength < 32)
        {
            nRemainder = nLength - 32;
            nLength = 32;
        }
        else
            nRemainder = 0;

        nData = (nDataBlock << 8) | nDataClass;
        gauge_cmd_write(CMD_DATA_CLASS, nData);

        if (gauge_write(CMD_BLOCK_DATA, pData, nLength) != nLength)
            return -1;

        pCheckSum[0] = check_sum(pData, nLength);
        gauge_write(CMD_CHECK_SUM, pCheckSum, 1);
        //usleep(10000);
        for(i = 0; i < 65535; i++);
        gauge_cmd_write(CMD_DATA_CLASS, nData);
        gauge_read(CMD_CHECK_SUM, pCheckSum + 1, 1);

        if (pCheckSum[0] != pCheckSum[1])
            return -2;

        pData += nLength;
        nDataBlock++;

    } while (nRemainder > 0);

return 0;
}

