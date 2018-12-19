//Code borrowed from GEODATASOURCE website.
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::                                                                         :*/
/*::  This routine calculates the distance between two points (given the     :*/
/*::  latitude/longitude of those points). It is being used to calculate     :*/
/*::  the distance between two locations using GeoDataSource(TM) products.   :*/
/*::                                                                         :*/
/*::  Definitions:                                                           :*/
/*::    South latitudes are negative, east longitudes are positive           :*/
/*::                                                                         :*/
/*::  Passed to function:                                                    :*/
/*::    lat1, lon1 = Latitude and Longitude of point 1 (in decimal degrees)  :*/
/*::    lat2, lon2 = Latitude and Longitude of point 2 (in decimal degrees)  :*/
/*::    unit = the unit you desire for results                               :*/
/*::           where: 'M' is statute miles (default)                         :*/
/*::                  'K' is kilometers                                      :*/
/*::                  'N' is nautical miles                                  :*/
/*::  Worldwide cities and other features databases with latitude longitude  :*/
/*::  are available at https://www.geodatasource.com                          :*/
/*::                                                                         :*/
/*::  For enquiries, please contact sales@geodatasource.com                  :*/
/*::                                                                         :*/
/*::  Official Web site: https://www.geodatasource.com                        :*/
/*::                                                                         :*/
/*::           GeoDataSource.com (C) All Rights Reserved 2017                :*/
/*::                                                                         :*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#define pi 3.14159265358979323846

#define MILES 0
#define KILOM 1

//maximum realistic point-to-point distance on earth (in meters), about 20 million meters or 20,000 kilometers
#define MAX_POSSIBLE_GPS_DISTANCE ((float)20000000)
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  Function prototypes                                           :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double);
double rad2deg(double);

typedef struct gps_diff
{
	char distanceunit;
	double distance;
	double angle; //N is 0 deg, orientation cw
} gpd_diff_t;

double distance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2, char unit, char factor_in_alt);

double angle(double lat1, double lon1, double lat2, double lon2);

int cardinal_dir_eight_wind(char* cardinal, float angle);

int cardinal_dir_sixteen_wind(char* cardinal, float angle);

int get_obfuscated_dist(char* dir_mesg, float dist, float angle);
