/*
 * modemparse.h
 *
 *  Created on: Dec 16, 2018
 *      Author: Vulcan
 */

#ifndef GPS_MODEMPARSE_H_
#define GPS_MODEMPARSE_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gps_decoder.h"
#include "utils/uart_nointerrupt.h"

int split_modpacket(char* str, gps_raw_t *gps);

#endif /* GPS_MODEMPARSE_H_ */
