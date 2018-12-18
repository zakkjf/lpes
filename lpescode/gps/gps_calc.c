//Code adapted from GEODATASOURCE website.
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::                                                                         :*/
/*::  This routine calculates the distance between two points (given the     :*/
/*::  latitude/lonitude of those points). It is being used to calculate     :*/
/*::  the distance between two locations using GeoDataSource(TM) products.   :*/
/*::                                                                         :*/
/*::  Definitions:                                                           :*/
/*::    South latitudes are negative, east lonitudes are positive           :*/
/*::                                                                         :*/
/*::  Passed to function:                                                    :*/
/*::    lat1, lon1 = Latitude and lonitude of point 1 (in decimal degrees)  :*/
/*::    lat2, lon2 = Latitude and lonitude of point 2 (in decimal degrees)  :*/
/*::    unit = the unit you desire for results                               :*/
/*::           where: 'M' is statute miles (default)                         :*/
/*::                  'K' is kilometers                                      :*/
/*::  Worldwide cities and other features databases with latitude lonitude  :*/
/*::  are available at https://www.geodatasource.com                          :*/
/*::                                                                         :*/
/*::  For enquiries, please contact sales@geodatasource.com                  :*/
/*::                                                                         :*/
/*::  Official Web site: https://www.geodatasource.com                        :*/
/*::                                                                         :*/
/*::           GeoDataSource.com (C) All Rights Reserved 2017                :*/
/*::                                                                         :*/
/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

#include <math.h>
#include "gps_calc.h"

double angle(double lat1, double lon1, double lat2, double lon2)
{

    double dLon = deg2rad(lon2 - lon1);

    double y = sin(dLon) * cos(deg2rad(lat2));
    double x = cos(deg2rad(lat1)) * sin(deg2rad(lat2)) - sin(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(dLon);

    double brng = atan2(y, x);

    brng = rad2deg(brng);
    brng = fmodf((brng + 360), 360); //make it positive
    //brng = 360 - brng; // count degrees counter-clockwise - remove to make clockwise

    return brng;
}

double distance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2, char unit, char factor_in_alt)
{
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dist = dist * 60 * 1.853159616;

  //factors in altitude in addition to lon/lat. This is really only significant for sudden altitude changes (like climbing the Flatirons)
  if(factor_in_alt) dist = sqrt(pow(dist*1000,2)+pow((alt2-alt1),2))/1000;

  switch(unit) {
    case MILES:
      dist = dist * 0.71550892786;
      break;
    case KILOM:
      break;
  }
  return (dist);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
  return (deg * pi / 180);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) {
  return (rad * 180 / pi);
}

int cardinal_dir_eight_wind(char* cardinal, float angle)
{
    /*
    Card 8  Direction   LowerBound  UpperBound
    N       0           337.5       22.5
    NE      45          22.5        67.5
    E       90          67.5        112.5
    SE      135         112.5       157.5
    S       180         157.5       202.5
    SW      225         202.5       247.5
    W       270         247.5       292.5
    NW      315         292.5       337.5
    */
    if(angle > 337.5 || angle <= 22.5)
    {
        strcpy(cardinal,"N");
    }
    else if (angle > 22.5 && angle <= 67.5)
    {
        strcpy(cardinal,"NE");
    }
    else if (angle > 67.5 && angle <= 112.5)
    {
        strcpy(cardinal,"E");
    }
    else if (angle > 112.5 && angle <= 157.5)
    {
        strcpy(cardinal,"SE");
    }
    else if (angle > 157.5 && angle <= 202.5)
    {
        strcpy(cardinal,"S");
    }
    else if (angle > 202.5 && angle <= 247.5)
    {
        strcpy(cardinal,"SW");
    }
    else if (angle > 247.5 && angle <= 292.5)
    {
        strcpy(cardinal,"W");
    }
    else if (angle > 292.5 && angle <= 337.5)
    {
        strcpy(cardinal,"NW");
    }
    else
    {
        strcpy(cardinal,"??");
        return 1;
    }
    return 0;
}

int cardinal_dir_sixteen_wind(char* cardinal, float angle)
{
    /*
    Card 16 Direction   LowerBound  UpperBound
    N       0           348.75      11.25
    NNE     22.5        11.25       33.75
    NE      45          33.75       56.25
    ENE     67.5        56.25       78.75
    E       90          78.75       101.25
    ESE     112.5       101.25      123.75
    SE      135         123.75      146.25
    SSE     157.5       146.25      168.75
    S       180         168.75      191.25
    SSW     202.5       191.25      213.75
    SW      225         213.75      236.25
    WSW     247.5       236.25      258.75
    W       270         258.75      281.25
    WNW     292.5       281.25      303.75
    NW      315         303.75      326.25
    NNW     337.5       326.25      348.75
    */

    if(angle > 348.75 || angle <= 11.25)
    {
        strcpy(cardinal,"N");
    }
    else if (angle > 11.25 && angle <= 33.75)
    {
        strcpy(cardinal,"NNE");
    }
    else if (angle > 33.75 && angle <= 56.25)
    {
        strcpy(cardinal,"NE");
    }
    else if (angle > 56.25 && angle <= 78.75)
    {
        strcpy(cardinal,"ENE");
    }
    else if (angle > 78.75 && angle <= 101.25)
    {
        strcpy(cardinal,"E");
    }
    else if (angle > 101.25 && angle <= 123.75)
    {
        strcpy(cardinal,"ESE");
    }
    else if (angle > 123.75 && angle <= 146.25)
    {
        strcpy(cardinal,"SE");
    }
    else if (angle > 146.25 && angle <= 168.75)
    {
        strcpy(cardinal,"SSE");
    }
    else if (angle > 168.75 && angle <= 191.25)
    {
        strcpy(cardinal,"S");
    }
    else if (angle > 191.25 && angle <= 213.75)
    {
        strcpy(cardinal,"SSW");
    }
    else if (angle > 213.75 && angle <= 236.25)
    {
        strcpy(cardinal,"SW");
    }
    else if (angle > 236.25 && angle <= 258.75)
    {
        strcpy(cardinal,"WSW");
    }
    else if (angle > 258.75 && angle <= 281.25)
    {
        strcpy(cardinal,"W");
    }
    else if (angle > 281.25 && angle <= 303.75)
    {
        strcpy(cardinal,"WNW");
    }
    else if (angle > 303.75 && angle <= 326.25)
    {
        strcpy(cardinal,"NW");
    }
    else if (angle > 326.25 && angle <= 348.75)
    {
        strcpy(cardinal,"NNW");
    }
    else
    {
        strcpy(cardinal,"???");
        return 1;
    }

    return 0;
}

int get_obfuscated_dist(char* dir_mesg, float dist, float angle)
{
    //Under 15m: "I'm very close to you! Look around!"
    //15m < d < 30m: "I'm very close by, somewhere <approx heading> !"
    //30m < d < 50m: "I'm in the area around here, somewhere <apx heading> !"
    //50m < d <100m: "I'm within walking distance, somewhere <apx heading> !"
    //100m < d <500m: "Come find me! I'm to the <apx heading> !"
    //500m < d < inf: "I'm pretty far away! Head <apx heading> !"
    char cardinal[4] = "    ";

    if(dist>500)
    {
        cardinal_dir_sixteen_wind(cardinal, angle);
        sprintf(dir_mesg,"I'm pretty far away! Head %s !@",cardinal);
    }
    else if(500 > dist && dist >= 100)
    {
        cardinal_dir_sixteen_wind(cardinal, angle);
        sprintf(dir_mesg,"Come find me! I'm to the %s !@",cardinal);
    }
    else if(100 > dist && dist >= 50)
    {
        cardinal_dir_sixteen_wind(cardinal, angle);
        sprintf(dir_mesg,"I'm within walking distance, somewhere %s !@",cardinal);
    }
    else if(50 > dist && dist >= 30)
    {
        cardinal_dir_eight_wind(cardinal, angle);
        sprintf(dir_mesg,"I'm in the area around here, somewhere %s !@",cardinal);
    }
    else if(30 > dist && dist >= 15)
    {
        cardinal_dir_eight_wind(cardinal, angle);
        sprintf(dir_mesg,"I'm very close by, somewhere %s !@",cardinal);
    }
    else if(15 > dist && dist >= 0)
    {
        sprintf(dir_mesg,"I'm very close to you! Look around!@");
    }
    else
    {
        sprintf(dir_mesg,"You are an invalid distance from the tracker. Something is wack.@");
        return 1;
    }

    return 0;
}
