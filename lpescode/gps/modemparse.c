#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gps_decoder.h"
#include "utils/uart_nointerrupt.h"

int split_modpacket(char* str, gps_raw_t *gps)
{
    //char str2[] = "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      ";
    char *r = strdup(str);
    // check for errors

    char *stor[5]; //larger than total fields for safety
    char i=0;
    char delim[] = ",";
    char* token;

    for (token = strtok(str, delim); token; token = strtok(NULL, delim))
    {
        stor[i++] = token;
    }
    gps->phone = atoll(stor[0]);
    gps->lat_dec_deg  = atof(stor[1]);
    gps->lon_dec_deg = atof(stor[2]);

    free(r);
    return 0;
}
