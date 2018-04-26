#include "gps_decoder.h"
#include "gps_calc.h"

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

    double apt[3] = {40.005779, -105.255646,1618.7};
	double home[3] = {39.059280, -84.327161,259.6};
	double apes[3] = {40.007035, -105.263579,1635.7};
	double north[3] = {90, 0, 0};
	double south[3] = {-90, 0, 0};
	double mumbai[3] = {19.087644, 72.867569, 6.0};
	double jep[3] = {39.849316, -104.673980,1646.8};
	double beer[3] = {40.016987, -105.280460,1633.2};
	double soops[3] = {40.015798, -105.251586,1606.4};
	double sanitas[3] = {40.03432,-105.30555,2072.4};



	unsigned char i, altfac;

	char* units[2] = {"Miles","Kilometers"};

	for(altfac=0;altfac<2;altfac++)
	{
		if(altfac)
			printf("\nDistances not factoring in altitude\n");
		else
			printf("\nDistances factoring in altitude\n");

		for(i=0;i<2;i++)
		{
			printf("\nDistances in %s:\n",units[i]);

			printf("Distance to Chhatrapati Shivaji International Airport in Mumbai, India %f %s\n", distance(mumbai[0], mumbai[1], mumbai[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

			printf("Distance to my house in Cincinnati, OH %f %s\n", distance(home[0], home[1], home[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);
		    
		    printf("Distance to Jeppesen Terminal, Denver International Airport %f %s\n", distance(jep[0], jep[1], jep[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

		    printf("Distance to the geographic north pole %f %s\n", distance(north[0], north[1], jep[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

		    printf("Distance to the geographic south pole %f %s\n", distance(south[0], south[1], south[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

		}

		printf("\nGranular distances in Meters:\n");

		printf("Distance to the APES lecture hall %f meters\n", distance(apes[0], apes[1], apes[3], gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);
	    
	    printf("Distance to my apartment in East Aurora %f meters\n", distance(apt[0], apt[1], apt[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);

	    printf("Distance to Boulder Beer on Pearl Street %f meters\n", distance(beer[0], beer[1], beer[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);

	    printf("Distance to King Sooper's Discount French Bread Cart in the Back of the Store %f meters\n", distance(soops[0], soops[1], soops[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);
	    
	    printf("Distance to the peak of Mt. Sanitas %f meters\n", distance(sanitas[0], sanitas[1], sanitas[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);

	}

	printf("Heading to the peak of Mt. Sanitas %f degrees CW of N\n", angle(gps.lat_dec_deg, gps.lon_dec_deg,sanitas[0], sanitas[1]));

	
    return 0;
}
