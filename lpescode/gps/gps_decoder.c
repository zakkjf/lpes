#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gps_decoder.h"
#include "utils/uart_nointerrupt.h"

char * strsep2 (char **stringp, const char *delim)
{
    char *begin, *end;
    begin = *stringp;
    if (begin == NULL) return NULL;
    // Find the end of the token.  
    end = begin + strcspn (begin, delim);

    if (*end)
    {
        //Terminate the token and set *STRINGP past NUL character.  
        *end++ = '\0';
        *stringp = end;
    }
    else
    {
        //No more delimiters; this is the last token.
        *stringp = NULL;
    }

    return begin;
}

float make_fractional(int val)
{
	if(val==0) return 0;

	while((val%10)==0)
	{
		//printf("val: %d\n", val);
		//sleep(1);
		val/=10;

	}

	float flt = val;

	while(flt>1)
	{
		flt/=10;
	}

	return flt;
}

int split_GPGGA(char* str, gps_raw_t *gps)
{
    //char str2[] = "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      ";
    char *r = strdup(str);
    // check for errors
    
    char *stor[25]; //larger than total fields for safety
    char i=0;
    char delim[] = ",.";
    char* token;

    for (token = strtok(str, delim); token; token = strtok(NULL, delim))
    {
        stor[i++] = token;
    }

    uint32_t time = atoi(stor[1]);
    gps->utc_s = time%100;
    time/=100;
    gps->utc_m = time%100;
    time/=100;
    gps->utc_h = time%100;
    
    //printf("%s",stor[1]);
    gps->lat_deg = atoi(stor[3])/100;
    gps->lat_sec = atoi(stor[3])%100 + make_fractional(atoi(stor[4]));
    gps->lat_hem = *stor[5];
    gps->lon_deg = atoi(stor[6])/100;
    gps->lon_sec = atoi(stor[6])%100 + make_fractional(atoi(stor[7]));
    gps->lon_hem = *stor[8];
    gps->fixq = atoi(stor[11]) + make_fractional(atoi(stor[12]));
    gps->sat_count = atoi(stor[10]);
    gps->hdilution = atoi(stor[13]);
    gps->altitude_m = atoi(stor[14]);


    gps->lat_dec_deg = ((double)gps->lat_deg) + (((double)gps->lat_sec)/60);
    gps->lon_dec_deg = ((double)gps->lon_deg) + (((double)gps->lon_sec)/60);

    //negative conversions for hemisphere
    if(gps->lat_hem == 'S') gps->lat_dec_deg *= -1;
    if(gps->lon_hem == 'W') gps->lon_dec_deg *= -1;


    free(r);
    return 0;
}
