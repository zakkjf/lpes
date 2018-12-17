//Battery Gauge Library
//V1.0
//© 2016 Texas Instruments Inc.
#ifndef __GAUGE_H
#define __GAUGE_H

#include <stdbool.h>
#include <string.h>
#include "utils/i2c_nointerrupt.h"

#define     I2C_GAUGE               1

#define SOFT_RESET                  0x0042
#define GAUGECMD_ADDR               0x0000
#define GAUGECMD_CONTROL_STATUS     0x0000
#define GAUGECMD_CONTROL_ID         0x0001
#define GAUGE_CMD_RESET             0x0041

// BQ35100 gas gauge I2C addresses
#define GAUGE_I2C_ADDR_WRITE        0x55 // 0xAA (the TiVa I2C drivers left-shift these by 1
#define GAUGE_I2C_ADDR_READ         0x55 // 0xAB

#define GAUGE_REG_MAC_WRITE         0x3E
#define GAUGE_REG_MAC_READ          0x40
#define GAUGE_REG_ID                0x0002
#define GAUGE_REG_SOH               0x002E
#define GAUGE_REG_VOLTAGE           0x0008



//gauge_read: read bytes from gauge (must be implemented for a specific system)
//pHandle: handle to communications adapater (removed)
//nRegister: first register (=standard command) to read from
//pData: pointer to a data buffer
//nLength: number of bytes
//return value: number of bytes read (0 if error)
int gauge_read(unsigned char nRegister, unsigned char *pData, unsigned char nLength);
//int gauge_read(uint8_t cmd_addr, uint8_t cmd_reg, uint8_t read_addr,
//               uint8_t* read_cmd, uint8_t read_length, uint8_t* data);


//gauge_write: write bytes to gauge (must be implemented for a specific system)
//pHandle: handle to communications adapater (removed)
//nRegister: first register (=standard command) to write to
//pData: pointer to a data buffer
//nLength: number of bytes
//return value: number of bytes written (0 if error)
int gauge_write(unsigned char nRegister, unsigned char *pData, unsigned char nLength);

//gauge_control: issues a sub command
//pHandle: handle to communications adapter
//nSubCmd: sub command number
//return value: result from sub command
unsigned int gauge_control(unsigned int nSubCmd);

//gauge_cmd_read: read data from standard command
//pHandle: handle to communications adapter
//nCmd: standard command
//return value: result from standard command
unsigned int gauge_cmd_read(unsigned char nCmd);

//gauge_cmd_write: write data to standard command
//pHandle: handle to communications adapter
//nCmd: standard command
//return value: number of bytes written to gauge
unsigned int gauge_cmd_write(unsigned char nCmd, unsigned int nData);

//gauge_cfg_update: enter configuration update mode for rom gauges
//pHandle: handle to communications adapter
//return value: true = success, false = failure
bool gauge_cfg_update();

//gauge_exit: exit configuration update mode for rom gauges
//pHandle: handle to communications adapter
//nSubCmd: sub command to exit configuration update mode
//return value: true = success, false = failure
bool gauge_exit(unsigned int nSubCmd);

//gauge_read_data_class: read a data class
//pHandle: handle to communications adapter
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_read_data_class(unsigned char nDataClass, unsigned char *pData,
                          unsigned char nLength);

//gauge_write_data_class: write a data class
//pHandle: handle to communications adapter
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_write_data_class(unsigned char nDataClass, unsigned char *pData,
                           unsigned char nLength);


#endif
