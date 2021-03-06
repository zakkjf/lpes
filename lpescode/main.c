
#define TARGET_IS_TM4C129_RA0
//#define DEBUG_OUT

#include "main.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"

//TI compiler stuff
#include "string.h"

#define ADAM_PHONE 8582319229
#define ZACH_PHONE 5136388369
#define MASTER_PHONE_1 ADAM_PHONE
#define MASTER_PHONE_2 0

#define BOARD_ID_VER   "TIVA:01:1"
#define FAKE_GPS 0 //switch on this mode if GPS is being difficult
#define PING_ALIVE 0
#define MAX_MOD_LENGTH 40
#define FAKE_GPS_DATA "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      " //MUST MATCH MSG_LEN!!!
#define ERRORCODE "$GPGGA,0.00,0.0,0,0.00,0,0,00,999.99,0,0,0,0,,*5C\n\r      "

#define GPS_BIT     0x01
#define IMU_BIT     0x02
#define MSG_LEN     83
#define KEY         '$'
#define ENDKEY      '\r'
#define GPS_TAG  "GPSDATA"
#define IMU_TAG  "IMUDATA"
#define TAGSIZE  8
#define BUFSIZE 100
char msg[MSG_LEN];
char modem_msg[MSG_LEN];
char str_msg[MSG_LEN];
uint32_t ui32Status;

gps_raw_t my_location;
gps_raw_t phone_location;
imu_raw_t imu_ptr;

CB_t modem_rx_buffer;
char modem_rx_data[MODEM_RX_BUFFER_SIZE_MAX][64];

void UARTIntHandler(void)
{
    static char grabkey = 0;
    static uint8_t mesg_i = 0;
    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART2_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART2_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    ROM_UARTCharPutNonBlocking(UART4_BASE, 'R');

    while(ROM_UARTCharsAvail(UART2_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        if(!grabkey)
        {
            if(KEY==ROM_UARTCharGetNonBlocking(UART2_BASE))
            {
                grabkey = 1;
            }
        }
        else
        {
            grabkey = ROM_UARTCharGetNonBlocking(UART2_BASE);

            if(grabkey==ENDKEY)
            {
                modem_msg[mesg_i++]='\0';
                if(CB_FULL!=CB_is_full(&modem_rx_buffer)) //make sure message buffer is not full
                {
                    CB_buffer_add_item(&modem_rx_buffer, modem_msg);
                }
                //memset(modem_msg,0,MSG_LEN);
                mesg_i=0;
                grabkey=0;
                //iterate to next item in buffer
                break;
            }
            else
            {
                modem_msg[mesg_i++] = grabkey;
            }
        }
    }

  //  CB_buffer_add_item(modem_rx_buffer, )
}
uint32_t checkBatt()
{
    char doop[200];
    unsigned char i2c_buffer[8];
    uint32_t bin32 = 0;
    uint8_t i2c_retry = 0;

    // The gauge seems to update its voltage only when it first powers up,
    // there must be some other trigger for it to update its voltage reading
 //   gauge_write_data_class(GAUGE_CMD_RESET, i2c_buffer, 4);

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

    return bin32;
}
// Main function
int main(void)
{
    char doop[200];

    uint32_t bin32 = 0;


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

    sendUARTstring(DEBUG_UART, "Start Debug\r\n@", 20);//command mode on modem

    init_gps(GPS_UART, SYSTEM_CLOCK);

    // Initialize I2C for gas gauge
    initi2c(GAUGE_I2C, SYSTEM_CLOCK);

    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    //parse GPS data and store it
    ///INITIALIZE EVERYTHING BELOW THIS LINE, I2C register calls MESS WITH HEAP ALLOCATION FOR SOME BIZARRE FUCKING REASON

    // Initialize circular buffer for Modem UART Rx
    CB_init(&modem_rx_buffer, &modem_rx_data, MODEM_RX_BUFFER_SIZE_MAX);

    ROM_UARTIntClear(UART2_BASE, ROM_UARTIntStatus(UART2_BASE, true));
    ROM_IntEnable(INT_UART2);
    ROM_UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);
    ROM_IntMasterEnable();

    while(1)
    {
        bin32 = checkBatt();
        memset(str_msg,' ',MSG_LEN);

        while(CB_EMPTY==CB_is_empty(&modem_rx_buffer));
        CB_buffer_remove_item(&modem_rx_buffer, str_msg);

        #if FAKE_GPS
                memcpy(msg, FAKE_GPS_DATA,MSG_LEN);
               // SysCtlDelay(SysCtlClockGet()/3);
        #else
                get_gps(GPS_UART,msg);
        #endif

        // Parse GPGGA packet
        memset(doop,0,BUFSIZE);
        sprintf(doop,"%s",msg);
        split_GPGGA(doop, &my_location);

        // Parse modem packet
        memset(doop,0,BUFSIZE);
        sprintf(doop,"%s",str_msg);
        split_modpacket(doop, &phone_location);
        SysCtlDelay(SysCtlClockGet()*3);
        sendUARTstring(MODEM_UART, "+++", 3);//command mode on modem
        SysCtlDelay(SysCtlClockGet()*3);
        //phone_location.phone = 2136409224;

        // Calculate recorded phone location distance to this device's GPS location
        float dist = distance(phone_location.lat_dec_deg, phone_location.lon_dec_deg, 0, my_location.lat_dec_deg, my_location.lon_dec_deg,0, 1, 0)*1000;
        float angl = angle(phone_location.lat_dec_deg, phone_location.lon_dec_deg, my_location.lat_dec_deg, my_location.lon_dec_deg);
        memset(doop,0,BUFSIZE);
        sprintf(doop,"ATP#%llu\r@",phone_location.phone);
        // Send command lines and result to modem
        sendUARTstring(MODEM_UART, doop, 20);//command mode on modem
        sendUARTstring(DEBUG_UART, doop, 20);//command mode on modem
        SysCtlDelay(SysCtlClockGet());
        sendUARTstring(MODEM_UART, "ATAC\r@",9);//command mode on modem
        SysCtlDelay(SysCtlClockGet());
        sendUARTstring(MODEM_UART, "ATCN\r@",9);//command mode on modem
        SysCtlDelay(SysCtlClockGet());
        memset(doop,0,BUFSIZE);
        if(phone_location.phone==MASTER_PHONE_1 || phone_location.phone==MASTER_PHONE_2)
        {
            sprintf(doop,"BOW BEFORE YOUR CREATOR MODE: Distance to target %.2f meters.@", dist);
            sendUARTstring(MODEM_UART, doop, 70);
            memset(doop,0,BUFSIZE);
            sprintf(doop,"Heading to target is %.2f degrees CW of N.@", angl);
            sendUARTstring(MODEM_UART, doop, 50);
            memset(doop,0,BUFSIZE);
            sprintf(doop, "%s%lumV\n\r@", " Battery starting voltage =", bin32);
            sendUARTstring(MODEM_UART, doop, 80);
        }
        else
        {
            memset(doop,0,BUFSIZE);
            get_obfuscated_dist(doop, dist, angl);
            sendUARTstring(MODEM_UART, doop, 80);
            memset(doop,0,BUFSIZE);
            sprintf(doop, "%s%lumV\n\r@", " Battery starting voltage =", bin32);
            sendUARTstring(MODEM_UART, doop, 80);
        }
/*
        SysCtlDelay(SysCtlClockGet()*3);
        sendUARTstring(2, "+++@", 9);//command mode on modem
        SysCtlDelay(SysCtlClockGet()*3);
        sendUARTstring(2, "ATFR\r@",8);//command mode on modem
*/
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
