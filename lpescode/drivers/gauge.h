//Battery Gauge Library
//V1.0
//© 2016 Texas Instruments Inc.
#ifndef __GAUGE_H
#define __GAUGE_H

#include <stdbool.h>

#define SOFT_RESET 0x0042
//gauge_control: issues a sub command
//pHandle: handle to communications adapter
//nSubCmd: sub command number
//return value: result from sub command
unsigned int gauge_control(void *pHandle, unsigned int nSubCmd);

//gauge_cmd_read: read data from standard command
//pHandle: handle to communications adapter
//nCmd: standard command
//return value: result from standard command
unsigned int gauge_cmd_read(void *pHandle, unsigned char nCmd);

//gauge_cmd_write: write data to standard command
//pHandle: handle to communications adapter
//nCmd: standard command
//return value: number of bytes written to gauge
unsigned int gauge_cmd_write(void *pHandle, unsigned char nCmd, unsigned int nData);

//gauge_cfg_update: enter configuration update mode for rom gauges
//pHandle: handle to communications adapter
//return value: true = success, false = failure
bool gauge_cfg_update(void *pHandle);

//gauge_exit: exit configuration update mode for rom gauges
//pHandle: handle to communications adapter
//nSubCmd: sub command to exit configuration update mode
//return value: true = success, false = failure
bool gauge_exit(void *pHandle, unsigned int nSubCmd);

//gauge_read_data_class: read a data class
//pHandle: handle to communications adapter
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_read_data_class(void *pHandle, unsigned char nDataClass, unsigned char *pData, unsigned
    char nLength);

//gauge_write_data_class: write a data class
//pHandle: handle to communications adapter
//nDataClass: data class number
//pData: buffer holding the whole data class (all blocks)
//nLength: length of data class (all blocks)
//return value: 0 = success
int gauge_write_data_class(void *pHandle, unsigned char nDataClass, unsigned char *pData,
                           unsigned char nLength);

//gauge_execute_fs: execute a flash stream file
//pHandle: handle to communications adapter
//pFS: zero-terminated buffer with flash stream file
//return value: success: pointer to end of flash stream file
//error: point of error in flashstream file
char *gauge_execute_fs(void *pHandle, char *pFS);

#endif
