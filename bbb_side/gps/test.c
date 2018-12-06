#include "gps_decoder.h"
#include "gps_calc.h"
#include "gps_dist.h"

#include <stdio.h>
#include <assert.h>

int main(void)
{
    //gps_raw_t gps_test;
    gps_raw_t gps;
    gps_raw_t gps_test;
    //gps_test.utc_time = 42655;
    gps_test.utc_h = 4;
    gps_test.utc_m = 26;
    gps_test.utc_s = 55;
    gps_test.lat_deg = 40;
    gps_test.lat_sec = 0.27270f;
    gps_test.lat_hem = 'N';
    gps_test.lon_deg = 105;
    gps_test.lon_sec = 15.37717f;
    gps_test.lon_hem = 'W';
    gps_test.fixq = 1;
    gps_test.sat_count = 4;
    gps_test.hdilution = 3;
    gps_test.altitude_m = 27;

	//char doo[] = "$GPGGA,223921.00,,,,,0,00,99.99,,,,,,*6F"; // a time-only fix. Will be interpreted as 0,0

    //char doo[] = "$GPGGA,022458.233,3854.932,N,07902.500,W,1,08,44,890,M,3900,M,,*51"; //somewhere in the middle of BFE West Virginia
    char doo[] = "$GPGGA,042655.00,4000.27270,N,10515.37717,W,1,04,3.27,1618.7,M,-21.3,M,,*51"; //roughly the office of my apartment complex
    split_GPGGA(doo,&gps);
    
    printf("NMEA PARSER UNIT TESTS:\n");
    printf("1 indicates PASS:\n");
    printf("time %d:%d:%d UTC\n",gps.utc_h == gps_test.utc_h, gps.utc_m == gps_test.utc_m, gps.utc_s == gps_test.utc_s);
    printf("lat %d\n",gps_test.lat_deg == gps.lat_deg);
    printf("%d\n",gps_test.lat_sec== gps.lat_sec);
    printf("lat hem %d\n",gps_test.lat_hem == gps.lat_hem);
    printf("lon %d\n",gps_test.lon_deg == gps.lon_deg);
    printf("%d\n",gps_test.lon_sec == gps.lon_sec);
    printf("long hem %d\n",gps_test.lon_hem == gps.lon_hem);
    printf("fixq %d\n",gps_test.fixq == gps.fixq);
    printf("sat %d\n",gps_test.sat_count == gps.sat_count);
    printf("h %d\n",gps_test.hdilution == gps.hdilution);
    printf("alt %d\n",gps_test.altitude_m == gps.altitude_m);

    printf("VALUES:\n");
    printf("time %d:%d:%d UTC\n",gps.utc_h,gps.utc_m,gps.utc_s);
    printf("lat dd %f\n",gps.lat_dec_deg);
    printf("lon dd %f\n",gps.lon_dec_deg);
    printf("lat %d deg %f' %c\n",gps.lat_deg,gps.lat_sec,gps.lat_hem);
    printf("lon %d deg %f' %c\n",gps.lon_deg,gps.lon_sec,gps.lon_hem);
    printf("fixq %d\n",gps.fixq);
    printf("sat %d\n",gps.sat_count);
    printf("h %d\n",gps.hdilution);
    printf("alt %d\n",gps.altitude_m);

    run_distances(gps,2);
    return 0;
}
