
#define TARGET_IS_TM4C129_RA0

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "drivers/pinout.h"
#include "utils/uart_nointerrupt.h"
#include "utils/i2c_nointerrupt.h"
#include "utils/mpu9250_regs.h"
#include "drivers/mpu9250.h"
#include "drivers/ubloxneo6.h"
#include "gps_decoder.h"
#include "gps_dist.h"
#include "gps_calc.h"
#include "modemparse.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"

//TI compiler stuff
#include "string.h"

#define BOARD_ID_VER   "TIVA:01:1"
#define FAKE_GPS 0 //switch on this mode if GPS is being difficult
#define PING_ALIVE 0
#define FAKE_GPS_DATA "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      " //MUST MATCH MSG_LEN!!!
#define ERRORCODE "$GPGGA,0.00,0.0,0,0.00,0,0,00,999.99,0,0,0,0,,*5C\n\r      "

//DON'T TOUCH STUFF BELOW THIS LINE//////////////////////////////////////////////////////

#include <main.h>

#define GPS_BIT     0x01
#define IMU_BIT     0x02
#define MSG_LEN     83

#define GPS_TAG  "GPSDATA"
#define IMU_TAG  "IMUDATA"
#define TAGSIZE  8

char msg[MSG_LEN];
char modem_msg[MSG_LEN];
gps_raw_t my_location;
gps_raw_t phone_location;
imu_raw_t imu_ptr;


// Main function
int main(void)
{
    char doop[100];
    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_16MHZ | SYSCTL_OSC_INT |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);

    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    initUART(2, 57600, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);
    init_gps(1,SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    //parse GPS data and store it
    while(1)
    {

        memset(doop,' ',100);
        getUARTlineOnKey(2, modem_msg,  MSG_LEN, '$');

        #if FAKE_GPS
                memcpy(msg, FAKE_GPS_DATA,MSG_LEN);
                SysCtlDelay(SysCtlClockGet()/3);
        #else
                get_gps(1,msg);
        #endif
        memset(doop,0,100);
        sprintf(doop,"%s",msg);
        split_GPGGA(doop, &my_location);
        //run_distances(my_location,0);
        memset(doop,0,100);
        sprintf(doop,"%s",modem_msg);
        split_modpacket(doop, &phone_location);
        SysCtlDelay(SysCtlClockGet()*4);
        sendUARTstring(2, "+++@", 9);//command mode on modem
        SysCtlDelay(SysCtlClockGet()*4);
        //phone_location.phone = 2136409224;
        float dist = distance(phone_location.lat_dec_deg, phone_location.lon_dec_deg, 0, my_location.lat_dec_deg, my_location.lon_dec_deg,0, 1, 0)*1000;
        float angl = angle(phone_location.lat_dec_deg, phone_location.lon_dec_deg, my_location.lat_dec_deg, my_location.lon_dec_deg);
        sprintf(doop,"ATP#%llu\r@",phone_location.phone);
        sendUARTstring(2, doop, 25);//command mode on modem
        sendUARTstring(2, "ATWR\r@",8);//command mode on modem
        sendUARTstring(2, "ATCN\r@",8);//command mode on modem

        //memset(doop,0,100);
        sprintf(doop,"Distance to target %f meters.@", dist);
        sendUARTstring(2, doop, 50);
        sprintf(doop,"Heading to target is %f degrees CW of N.\r@", angl);
        sendUARTstring(2, doop, 50);
    }

    return 0;
}

/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
