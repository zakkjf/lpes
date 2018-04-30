//Adapted from https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
//by Zach Farmer

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include "gps/gps_decoder.h"
#include <unistd.h>

#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED
	typedef struct imu_data{
	    int16_t x_accel;
	    int16_t y_accel;
	    int16_t z_accel;
	    int16_t x_gyro;
	    int16_t y_gyro;
	    int16_t z_gyro;
	    int16_t x_mag;
	    int16_t y_mag;
	    int16_t z_mag;
	    int16_t tempc;
	} imu_raw_t;


int split_packet(char* str, imu_raw_t *imu, gps_raw_t *gps);

#endif

