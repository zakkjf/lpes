#ifndef MAIN_H_
#define MAIN_H_

#define SYSTEM_CLOCK    120000000U


#define TARGET_IS_TM4C129_RA0

#define BOARD_ID_VER   "TIVA:01:1"
#define FAKE_GPS        1 //switch on this mode if GPS is being difficult
#define FAKE_GPS_DATA   "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      " //MUST MATCH MSG_LEN!!!
#define ERRORCODE       "$GPGGA,0.00,0.0,0,0.00,0,0,00,999.99,0,0,0,0,,*5C\n\r    "

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drivers/pinout.h"
#include "utils/uart_nointerrupt.h"
#include "utils/i2c_nointerrupt.h"
#include "utils/mpu9250_regs.h"
#include "drivers/mpu9250.h"
#include "drivers/ubloxneo6.h"
#include "gps_decoder.h"
#include "gps_dist.h"
#include "types.h"
#include "gauge.h"

#endif /* MAIN_H_ */
