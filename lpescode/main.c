
#define TARGET_IS_TM4C129_RA0
#define DEBUG_OUT

#include "main.h"

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

// Array of contacts received since power-on
Contacts_t Contact_List[CONTACTS_MAX];
volatile uint8_t i_Contacts = 0;

// Main function
int main(void)
{
    const uint16_t BUFSIZE = 100;
    char doop[BUFSIZE];
    unsigned char i2c_buffer[8];
    uint32_t bin32 = 0;
    uint8_t i2c_retry = 0;

    Contacts_t new_contact;

    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_16MHZ | SYSCTL_OSC_INT |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);

    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    // Initialize UART ports
    initUART(DEBUG_UART, DEBUG_UART_BAUD, SYSTEM_CLOCK, UART_CONFIG_PAR_NONE);
    initUART(MODEM_UART, MODEM_UART_BAUD, SYSTEM_CLOCK, UART_CONFIG_PAR_NONE);
    init_gps(GPS_UART, SYSTEM_CLOCK);

    // Initialize I2C for gas gauge
    initi2c(GAUGE_I2C, SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    // Check whether gas gauge is connected and responsive
    for(i2c_retry = 0; i2c_retry < I2C_RETRY_MAX; i2c_retry++)
    {
        gauge_read_data_class(GAUGE_REG_ID, i2c_buffer, 4);

        bin32 = ( (i2c_buffer[0] << 24) + (i2c_buffer[1] << 16) +
                (i2c_buffer[2] << 8) + i2c_buffer[3] );

        if(bin32 == GAUGE_ID)
        {
#ifdef DEBUG_OUT
            sprintf(doop, "\r\n%s\r\n", "Gas Gauge initialized");
#endif
            break;
        }
        else
        {
#ifdef DEBUG_OUT
            sprintf(doop, "\r\n%s%d\r\n", "Gas Gauge init fail, response = ", bin32);
#endif
        }

    }

#ifdef DEBUG_OUT
    sendUARTstring(DEBUG_UART, doop, strlen(doop));
#endif

    memset(doop, 0, BUFSIZE);
    for(i2c_retry = 0; i2c_retry < I2C_RETRY_MAX; i2c_retry++)
    {
        bin32 = gauge_cmd_read(GAUGE_REG_VOLTAGE);

        // If the reading is the wrong order of magnitude, retry
        if(bin32 < 1000 || bin32 > 10000)
        {
            continue;
        }
        else
        {
            break;
        }
    }

    sprintf(doop, "%s%lumV\r\n", "Battery voltage = ", bin32);
    sendUARTstring(DEBUG_UART, doop, strlen(doop));

    ArrayList_Init(Contact_List);

    //parse GPS data and store it
    while(1)
    {

        memset(doop,' ',BUFSIZE);
        getUARTlineOnKey(MODEM_UART, modem_msg,  MSG_LEN, '$');

        #if FAKE_GPS
                memcpy(msg, FAKE_GPS_DATA,MSG_LEN);
                SysCtlDelay(SysCtlClockGet()/3);
        #else
                get_gps(1,msg);
        #endif

                // Parse GPGGA packet
        memset(doop,0,BUFSIZE);
        sprintf(doop,"%s",msg);
        split_GPGGA(doop, &my_location);
        //run_distances(my_location,0);

        // Parse modem packet
        memset(doop,0,BUFSIZE);
        sprintf(doop,"%s",modem_msg);
        split_modpacket(doop, &phone_location);
        SysCtlDelay(SysCtlClockGet()*4);
        sendUARTstring(MODEM_UART, "+++@", 9);//command mode on modem
        SysCtlDelay(SysCtlClockGet()*4);
        //phone_location.phone = 2136409224;

        // Calculate recorded phone location distance to this device's GPS location
        float dist = distance(phone_location.lat_dec_deg, phone_location.lon_dec_deg, 0, my_location.lat_dec_deg, my_location.lon_dec_deg,0, 1, 0)*1000;
        float angl = angle(phone_location.lat_dec_deg, phone_location.lon_dec_deg, my_location.lat_dec_deg, my_location.lon_dec_deg);
        sprintf(doop,"ATP#%llu\r@",phone_location.phone);

        // Send command lines and result to modem
        sendUARTstring(MODEM_UART, doop, 25);//command mode on modem
        sendUARTstring(MODEM_UART, "ATWR\r@",8);//command mode on modem
        sendUARTstring(MODEM_UART, "ATCN\r@",8);//command mode on modem

        //memset(doop,0,BUFSIZE);
        sprintf(doop,"Distance to target %f meters.@", dist);
        sendUARTstring(MODEM_UART, doop, 50);
        sprintf(doop,"Heading to target is %f degrees CW of N.\r@", angl);
        sendUARTstring(MODEM_UART, doop, 50);
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
