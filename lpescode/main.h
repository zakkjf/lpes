#ifndef MAIN_H_
#define MAIN_H_

#define SYSTEM_CLOCK    120000000U

#define GPS_UART            1
#define GPS_UART_BAUD       9600

#define MODEM_UART          2
#define MODEM_UART_BAUD     57600

#define DEBUG_UART          4
#define DEBUG_UART_BAUD     115200

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
#include "arraylist.h"

#endif /* MAIN_H_ */
