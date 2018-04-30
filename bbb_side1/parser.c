#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include "gps/gps_decoder.h"
#include "parser.h"

int split_packet(char* str, imu_raw_t *imu, gps_raw_t *gps)
{
    char *r = strdup(str);
    // check for errors
    
    char *stor[3]; //larger than total fields for safety
    char *tok = r, *end = r;
    unsigned char i=0;
    
    while (tok != NULL) {
        strsep(&end, ":$");
        stor[i++]=tok;
        tok = end;
    }

    //printf("%s\n",stor[2]);
    //printf("%s\n",stor[1]);
    imu->z_accel = atoi(stor[1]);

    split_GPGGA(stor[2], gps);

    free(r);

    return 0;
}
