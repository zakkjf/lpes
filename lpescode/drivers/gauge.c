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

#include <string.h>
#include "gauge.h"

#define SET_CFGUPDATE 0x0013
#define CMD_DATA_CLASS 0x3E
#define CMD_BLOCK_DATA 0x40
#define CMD_CHECK_SUM 0x60
#define CMD_FLAGS 0x06
#define CFGUPD 0x0010

//gauge_read: read bytes from gauge (must be implemented for a specific system)
//pHandle: handle to communications adapater
//nRegister: first register (=standard command) to read from
//pData: pointer to a data buffer
//nLength: number of bytes
//return value: number of bytes read (0 if error)
extern int gauge_read(void *pHandle, unsigned char nRegister, unsigned char *pData, unsigned char
                      nLength);

//gauge_write: write bytes to gauge (must be implemented for a specific system)
//pHandle: handle to communications adapater
//nRegister: first register (=standard command) to write to
//pData: pointer to a data buffer
//nLength: number of bytes
//return value: number of bytes written (0 if error)
extern int gauge_write(void *pHandle, unsigned char nRegister, unsigned char *pData, unsigned
                       char nLength);

//gauge_address: set device address for gauge (must be implemented for a specific system; not required for HDQ)
//pHandle: handle to communications adapater
//nAddress: device address (e.g. 0xAA)
extern void gauge_address(void *pHandle, unsigned char nAddress);

//gauge_control: issues a sub command
//pHandle: handle to communications adapter
//nSubCmd: sub command number
//return value: result from sub command
unsigned int gauge_control(void *pHandle, unsigned int nSubCmd)
{
    unsigned int nResult = 0;
    char pData[2];

    pData[0] = nSubCmd & 0xFF;
    pData[1] = (nSubCmd >> 8) & 0xFF;

    gauge_write(pHandle, 0x00, pData, 2); // issue control and sub command
    gauge_read(pHandle, 0x00, pData, 2); // read data
    nResult = (pData[1] <<8) | pData[0];

    return nResult;
}

//gauge_cmd_read: read data from standard command
//pHandle: handle to communications adapter
//nCmd: standard command
//return value: result from standard command
unsigned int gauge_cmd_read(void *pHandle, unsigned char nCmd)
{
    unsigned char pData[2];

    gauge_read(pHandle, nCmd, pData, 2);

    return (pData[1] << 8) | pData[0];
}

//gauge_cmd_write: write data to standard command
//pHandle: handle to communications adapter
//nCmd: standard command
//return value: number of bytes written to gauge
unsigned int gauge_cmd_write(void *pHandle, unsigned char nCmd, unsigned int nData)
{
    unsigned char pData[2];

    pData[0] = nData & 0xFF;
    pData[1] = (nData >> 8) & 0xFF;

    return gauge_write(pHandle, nCmd, pData, 2);
}

//gauge_cfg_update: enter configuration update mode for rom gauges
//pHandle: handle to communications adapter
//return value: true = success, false = failure
#define MAX_ATTEMPTS 5
bool gauge_cfg_update(void *pHandle)
{
    unsigned int nFlags;
    int nAttempts = 0;

    gauge_control(pHandle, SET_CFGUPDATE);

    do
    {
        nFlags = gauge_cmd_read(pHandle, CMD_FLAGS);
        if (!(nFlags & CFGUPD)) usleep(500000);
    } while (!(nFlags & CFGUPD) && (nAttempts++ < MAX_ATTEMPTS));

    return (nAttempts < MAX_ATTEMPTS);
}

//gauge_exit: exit configuration update mode for rom gauges
//pHandle: handle to communications adapter
//nSubCmd: sub command to exit configuration update mode
//return value: true = success, false = failure
bool gauge_exit(void *pHandle, unsigned int nSubCmd)
{
    unsigned int nFlags;
    int nAttempts = 0;

    gauge_control(pHandle, nSubCmd);

    do
    {
        nFlags = gauge_cmd_read(pHandle, CMD_FLAGS);
        if (nFlags & CFGUPD) usleep(500000);
    } while ((nFlags & CFGUPD) && (nAttempts++ <MAX_ATTEMPTS));

    return (nAttempts < MAX_ATTEMPTS);
}

//gauge_read_data_class: read a data class
//pHandle: handle to communications adapter
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_read_data_class(void *pHandle, unsigned char nDataClass, unsigned char *pData, unsigned
                          char nLength)
{
    unsigned char nRemainder = nLength;
    unsigned int nOffset = 0;
    unsigned char nDataBlock = 0x00;
    unsigned int nData;
    if (nLength < 1) return 0;
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
        gauge_cmd_write(pHandle, CMD_DATA_CLASS, nData);

        if (gauge_read(pHandle, CMD_BLOCK_DATA, pData, nLength) != nLength)
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
//pHandle: handle to communications adapter
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_write_data_class(void *pHandle, unsigned char nDataClass, unsigned char *pData,
                           unsigned char nLength)
{
    unsigned char nRemainder = nLength;
    unsigned int nOffset = 0;
    unsigned char pCheckSum[2] = {0x00, 0x00};
    unsigned int nData;
    unsigned char nDataBlock = 0x00;

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
        gauge_cmd_write(pHandle, CMD_DATA_CLASS, nData);

        if (gauge_write(pHandle, CMD_BLOCK_DATA, pData, nLength) != nLength)
            return -1;

        pCheckSum[0] = check_sum(pData, nLength);
        gauge_write(pHandle, CMD_CHECK_SUM, pCheckSum, 1);
        usleep(10000);
        gauge_cmd_write(pHandle, CMD_DATA_CLASS, nData);
        gauge_read(pHandle, CMD_CHECK_SUM, pCheckSum + 1, 1);

        if (pCheckSum[0] != pCheckSum[1])
            return -2;

        pData += nLength;
        nDataBlock++;

    } while (nRemainder > 0);

return 0;
}

//gauge_execute_fs: execute a flash stream file
//pHandle: handle to communications adapter
//pFS: zero-terminated buffer with flash stream file
//return value: success: pointer to end of flash stream file
//error: point of error in flash stream file
char *gauge_execute_fs(void *pHandle, char *pFS)
{
    int nLength = strlen(pFS);
    int nDataLength;
    char pBuf[16];
    char pData[32];
    int n, m;
    char *pEnd = NULL;
    char *pErr;
    bool bWriteCmd = false;
    unsigned char nRegister;

    m = 0;
    for (n = 0; n < nLength; n++)
        if (pFS[n] != ' ') pFS[m++] = pFS[n];

    pEnd = pFS + m;
    pEnd[0] = 0;

    do
    {
        switch (*pFS)
        {
        case ';':
            break;
        case 'W':
        case 'C':
            bWriteCmd = *pFS == 'W';
            pFS++;

            if ((*pFS) != ':')
                return pFS;

            pFS++;

            n = 0;
            while ((pEnd - pFS > 2) && (n < sizeof(pData) + 2) &&(*pFS != '\n'))
            {
                pBuf[0] = *(pFS++);
                pBuf[1] = *(pFS++);
                pBuf[2] = 0;
                m = strtoul(pBuf, &pErr, 16);
                if (*pErr) return (pFS - 2);
                if (n == 0) gauge_address(pHandle, m);
                if (n == 1) nRegister = m;
                if (n > 1) pData[n - 2] = m;
                n++;
            }

            if (n < 3)
                return pFS;

            nDataLength = n - 2;

            if (bWriteCmd)
                gauge_write(pHandle, nRegister, pData, nDataLength);
            else
            {
                char pDataFromGauge[nDataLength];
                gauge_read(pHandle, nRegister, pDataFromGauge, nDataLength);
                if (memcmp(pData, pDataFromGauge, nDataLength)) return pFS;
            }

            break;

        case 'X':

            pFS++;
            if ((*pFS) != ':')
                return pFS;

            pFS++;

            n = 0;
            while ((pFS != pEnd) && (*pFS != '\n') &&(n <sizeof(pBuf) - 1))
            {
                pBuf[n++] = *pFS;
                pFS++;
            }

            pBuf[n] = 0;
            n = atoi(pBuf);
            usleep(n * 100);

            break;

        default:
            return pFS;

        }

        while ((pFS != pEnd) && (*pFS != '\n'))
            pFS++; //skip to next line

        if (pFS != pEnd)
            pFS++;

    } while (pFS != pEnd);

    return pFS;
}

