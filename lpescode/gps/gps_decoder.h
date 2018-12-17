#include<stdio.h>
#include<string.h>
#include<stdint.h>


#ifndef GPS_DECODER_H_INCLUDED
#define GPS_DECODER_H_INCLUDED

typedef struct gps_data{
    //time
    uint32_t phone;
    uint8_t utc_h;
    uint8_t utc_m;
    uint8_t utc_s;
    //dec deg
    double lat_dec_deg;
    double lon_dec_deg;
    //deg/sec/hemisphere data
    uint16_t lat_deg;
    float lat_sec;
    char lat_hem;
    uint16_t lon_deg;
    float lon_sec;
    char lon_hem;
    //quality data
    float fixq;
    uint16_t sat_count;
    uint16_t hdilution;
    //altitude
    uint16_t altitude_m;
} gps_raw_t;

int split_GPGGA(char* str, gps_raw_t *gps);

char * strsep2 (char **stringp, const char *delim);

#endif
