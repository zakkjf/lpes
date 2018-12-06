#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include "gps/gps_decoder.h"
#include "parser.h"

int split_packet(char* str, char* board, char* id, char* ver, imu_raw_t *imu, gps_raw_t *gps)
{
    char *r = strdup(str);
    // check for errors
    
    char *stor[7]; //larger than total fields for safety
    char *tok = r, *end = r;
    unsigned char i=0;
    
    while (tok != NULL) {
        strsep(&end, ":$");
        stor[i++]=tok;
        tok = end;
    }

    //printf("%s\n",stor[2]);
    //printf("%s\n",stor[1]);
    memcpy(board,stor[0],4); //board
    *id = atoi(stor[1]); //id
    *ver = atoi(stor[2]); //version

    //data below
    imu->z_accel = atoi(stor[3]);

    split_GPGGA(stor[4], gps);

    free(r);

    return 0;
}
