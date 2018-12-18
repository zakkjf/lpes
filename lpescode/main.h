#ifndef MAIN_H_
#define MAIN_H_

#define SYSTEM_CLOCK    120000000U

#define GPS_UART            1
#define GPS_UART_BAUD       9600

#define MODEM_UART          2
#define MODEM_UART_BAUD     57600

#define DEBUG_UART          4
#define DEBUG_UART_BAUD     115200

#define GAUGE_I2C           1
#define GAUGE_ID            0x1000102

#define I2C_RETRY_MAX       64

#define MODEM_RX_BUFFER_SIZE_MAX    10

/*// Trying to write a macro for i2c transaction repeats since we're experiencing unreliable comm
#define I2C_TRANSACT_TIMEOUT(n, x) ( memset(doop, 0, BUF_SIZE);\
    for(timeout = 0, bin32 = 0; timeout < TIMEOUT_MAX; timeout++) { n;\
    bin32 = (uint32_t)( (doop[0] << 24) | (uint32_t)(doop[1] << 16) | (uint32_t)(doop[2] << 8) | (uint32_t)doop[3] );\
    if(bin32 == x) break;\ } )
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "drivers/pinout.h"
#include "utils/uart_nointerrupt.h"
#include "utils/i2c_nointerrupt.h"
#include "utils/mpu9250_regs.h"
#include "drivers/mpu9250.h"
#include "drivers/ubloxneo6.h"
#include "gps_decoder.h"
#include "gps_dist.h"
#include "gps_calc.h"
#include "modemparse.h"
#include "gauge.h"
#include "utils/arraylist.h"
#include "utils/circbuf.h"

extern CB_t modem_rx_buffer;
extern char modem_rx_data[MODEM_RX_BUFFER_SIZE_MAX][64];

void UARTIntHandler(void);

#endif /* MAIN_H_ */
