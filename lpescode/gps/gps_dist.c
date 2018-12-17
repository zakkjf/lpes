#include "gps_decoder.h"
#include "gps_calc.h"
#include "gps_dist.h"

#include "utils/uart_nointerrupt.h"

#include <stdio.h>
#include <assert.h>

int run_distances(gps_raw_t gps, uint8_t choice)
{
	if(choice>2) return 1; //not a valid choice

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

	if(choice == 0)
	{
		uprintf(2,"Distance to the LPES lecture hall %f meters. ", distance(apes[0], apes[1], apes[3], gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, 0)*1000);
		uprintf(2,"Heading to the LPES lecture hall %f degrees CW of N\n\r", angle(gps.lat_dec_deg, gps.lon_dec_deg,apes[0], apes[1]));
	}
	else
	{
		unsigned char i, altfac;

		char* units[2] = {"Miles","Kilometers"};

		for(altfac=0;altfac<choice;altfac++)
		{
			if(altfac)
				uprintf(2,"\nDistances not factoring in altitude\n");
			else
				uprintf(2,"\nDistances factoring in altitude\n");

			for(i=0;i<2;i++)
			{
				uprintf(2,"\nDistances in %s:\n",units[i]);

				uprintf(2,"Distance to Chhatrapati Shivaji International Airport in Mumbai, India %f %s\n", distance(mumbai[0], mumbai[1], mumbai[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

				uprintf(2,"Distance to my house in Cincinnati, OH %f %s\n", distance(home[0], home[1], home[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);
			    
			    uprintf(2,"Distance to Jeppesen Terminal, Denver International Airport %f %s\n", distance(jep[0], jep[1], jep[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

			    uprintf(2,"Distance to the geographic north pole %f %s\n", distance(north[0], north[1], jep[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

			    uprintf(2,"Distance to the geographic south pole %f %s\n", distance(south[0], south[1], south[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, i, altfac),  units[i]);

			}

			uprintf(2,"\nGranular distances in Meters:\n");
		    
		    uprintf(2,"Distance to my apartment in East Aurora %f meters\n", distance(apt[0], apt[1], apt[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);

		    uprintf(2,"Distance to Boulder Beer on Pearl Street %f meters\n", distance(beer[0], beer[1], beer[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);

		    uprintf(2,"Distance to King Sooper's Discount French Bread Cart in the Back of the Store %f meters\n", distance(soops[0], soops[1], soops[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);
		    
		    uprintf(2,"Distance to the peak of Mt. Sanitas %f meters\n", distance(sanitas[0], sanitas[1], sanitas[3],gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, altfac)*1000);

		}

		uprintf(2,"Heading to the peak of Mt. Sanitas %f degrees CW of N\n", angle(gps.lat_dec_deg, gps.lon_dec_deg,sanitas[0], sanitas[1]));
	}
    return 0;
}
