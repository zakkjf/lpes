/*
 * Low Power Embedded Systems Fall 2018
 * Adam Nuhaily and Zach Farmer
 * whereis.it hide-and-seek / homing device code
 *
 * This FreeRTOS-based software implements the functionality for our hide-and-seek game
 * for the class to play.
 */

#define GPS_BIT     0x01
#define IMU_BIT     0x02
#define MSG_LEN     83

#define GPS_TAG  "GPSDATA"
#define IMU_TAG  "IMUDATA"
#define TAGSIZE  8

#include "main.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"
#include "driverlib/i2c.h"

//TI compiler stuff
#include "string.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"

#define BOARD_ID_VER   "TIVA:01:1"
#define FAKE_GPS 1 //switch on this mode if GPS is being difficult
#define PING_ALIVE 0
#define FAKE_GPS_DATA "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      " //MUST MATCH MSG_LEN!!!
#define ERRORCODE "$GPGGA,0.00,0.0,0,0.00,0,0,00,999.99,0,0,0,0,,*5C\n\r      "

//DON'T TOUCH STUFF BELOW THIS LINE//////////////////////////////////////////////////////

#define GPS_BIT     0x01
#define IMU_BIT     0x02
#define MSG_LEN     83

#define GPS_TAG  "GPSDATA"
#define IMU_TAG  "IMUDATA"
#define TAGSIZE  8

SemaphoreHandle_t xSemaphore1;
SemaphoreHandle_t xSemaphore2;
SemaphoreHandle_t xSemaphore3;

QueueHandle_t xQueue1;

//Task declarations
void task_init(void *pvParameters);
void task_imu(void *pvParameters);
void task_gps(void *pvParameters);
void task_comm(void *pvParameters);

void vTimerCallback1(TimerHandle_t xTimer);

char msg[MSG_LEN];
char modem_msg[MSG_LEN];
gps_raw_t my_location;
imu_raw_t imu_ptr;

//TaskHandle_t th0;
TaskHandle_t th1;
TaskHandle_t th2;
TaskHandle_t th3;

/* An array to hold handles to the created timers. */
TimerHandle_t xTimer;

uint32_t tick_counter;

// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    char temp_buffer[128];

    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_16MHZ | SYSCTL_OSC_INT |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);

    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    xSemaphore1 = xSemaphoreCreateMutex();
    xSemaphore2 = xSemaphoreCreateMutex();
    xSemaphore3 = xSemaphoreCreateMutex();

    initUART(2, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);

    tick_counter = 0;


    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    initUART(DEBUG_UART, 115200, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);
    initUART(2, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);

    // Initialize I2C
    initi2c(I2C_GAUGE, SYSTEM_CLOCK);

    xQueue1 = xQueueCreate( 10, sizeof(uint32_t) );

    // Create tasks
  //  xTaskCreate(task_init, (const portCHAR *)"INIT",
 //               configMINIMAL_STACK_SIZE, NULL, 1, &th0);
    xTaskCreate(task_imu, (const portCHAR *)"IMU",
                configMINIMAL_STACK_SIZE, NULL, 1, &th1);
    xTaskCreate(task_gps, (const portCHAR *)"GPS",
                configMINIMAL_STACK_SIZE, NULL, 1, &th2);
    xTaskCreate(task_comm, (const portCHAR *)"COMM",
                configMINIMAL_STACK_SIZE, NULL, 1, &th3);

    xTimer = xTimerCreate("Timer0", 250, pdTRUE, ( void * ) 0, vTimerCallback1);

    if( xTimerStart( xTimer, 0 ) != pdPASS )
    {
        //The timer could not be set into the Active state.
    }

    vTaskStartScheduler();

    return 0;
}


// Startup items, run once and kill task
void task_init(void *pvParameters)
{
   // char temp_buffer[128];



    return;
}

// Flash the LEDs on the launchpad
void task_imu(void *pvParameters)
{
    //initi2c(2, SYSTEM_CLOCK);
    //initMPU9250(2,MPU9250_ADDRESS_1);
   // assert_mag(2,AK8963_ADDRESS); //startup tests
   // assert_MPU(2,MPU9250_ADDRESS_1);

    char doop[100];
    uint16_t battery_voltage = 0;
    uint8_t gaugeData[4];


    for (;;)
    {
        if( xSemaphore1 != NULL )
        {

            if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
               // get_sensors(2,MPU9250_ADDRESS_1,&imu_ptr);
                //
                xSemaphoreGive( xSemaphore );

                //     sendi2cbytes(I2C_GAUGE, GAUGE_I2C_ADDR_WRITE, GAUGE_REG_MAC_WRITE, 2, gaugeData);


                //     fetchi2cbytes(I2C_GAUGE, GAUGE_I2C_ADDR_WRITE, GAUGE_I2C_ADDR_READ, GAUGE_REG_MAC_READ, 4, gaugeData);
               //      gauge_read_data_class(GAUGE_REG_ID, gaugeData, 4);

                  //   battery_voltage = gauge_cmd_read(GAUGE_REG_VOLTAGE);

                 //    sprintf(doop, "Device ID returned: 0x%d%d\r\n", gaugeData[0], gaugeData[1]);
                   //  sendUARTstring(DEBUG_UART, doop, 100);

                  //   sprintf(doop, "Battery voltage: %dmV\r\n", battery_voltage);
                    // sendUARTstring(DEBUG_UART, doop, 100);
                //vTaskDelay(1);
                xTaskNotify(th3, IMU_BIT, eSetBits);
                vTaskSuspend(th1);

            if( xSemaphoreTake( xSemaphore1, ( TickType_t ) 10 ) == pdTRUE )
            {
               // get_sensors(2,MPU9250_ADDRESS_1,&imu_ptr);
                vTaskDelay(1000);
                xTaskNotify(th3, IMU_BIT, eSetBits);
            }
        }
    }
}

// Flash the LEDs on the launchpad
void task_gps(void *pvParameters)
{
    for(;;)
    {
#if FAKE_GPS
        memcpy(msg, FAKE_GPS_DATA, strlen(FAKE_GPS_DATA) );//FAKE_GPS_DATA,MSG_LEN);
        vTaskDelay(1000);
#else
        get_gps(1,msg);
#endif
        msg[82]='\0';
        tick_counter++;

        if( xQueue1 != 0 )

        //check for incoming msg
        if( xSemaphore2 != NULL )
        {
            if( xSemaphoreTake( xSemaphore2, ( TickType_t ) 10 ) == pdTRUE )
            {
                getUARTline(2, modem_msg, MSG_LEN);

                xTaskNotify(th3, GPS_BIT, eSetBits);
            }
        }
    }
}

//communication task, sends data over uart to master device using EVEN parity, and to terminal using no parity
void task_comm(void *pvParameters)
{
    uint8_t gaugeData[4];
    uint8_t i = 0;

    //initUART(2, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);
    //initUART(6, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_EVEN);

    //initUART(6, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_EVEN);
    init_gps(1,SYSTEM_CLOCK);



  //  gaugeData[0] = 0xAB;
   // gaugeData[1] = 0;
 //   gaugeData[2] = 1;

   // sendi2cbytes(I2C_GAUGE, GAUGE_I2C_WRITE_ADDR, 0x00, 3, gaugeData);
  //  geti2cbytes(I2C_GAUGE, GAUGE_I2C_WRITE_ADDR, 0x00, 2, gaugeData);

  //  gauge_read(GAUGECMD_CONTROL_ID, gaugeData, )

    const TickType_t xMaxBlockTime = 5000;
    BaseType_t xResult;
    uint32_t ulNotifiedValue;
    char toggleLED = 0;
    char doop[100];

    uint16_t battery_voltage = 0; // voltage in mV

    for( ;; )
    {
        /* Wait to be notified of an interrupt. */
        xResult = xTaskNotifyWait( 0x0000,    /* Don't clear bits on entry. */
                                    0xFFFF,        /* Clear all bits on exit. */
                                    &ulNotifiedValue, /* Stores the notified value. */
                                    xMaxBlockTime );

        if( xResult == pdPASS )
        {
            memset(doop,'\0',sizeof(doop));
            /* A notification was received.  See which bits were set. */
            if( ( ulNotifiedValue & IMU_BIT)!= 0 )
            {

            }

            if( ( ulNotifiedValue & GPS_BIT ) != 0 )
            {
               //parse GPS data and store it

                #if FAKE_GPS
                        memcpy(msg, FAKE_GPS_DATA,MSG_LEN);
                        vTaskDelay(1000);
                #else
                        get_gps(1,msg);
                #endif
               sprintf(doop,"%s",msg);
               split_GPGGA(doop, &my_location);
               //run_distances(my_location,0);
               sendUARTstring(2, "TEST\n\r", 7);
               xSemaphoreGive( xSemaphore1);
               xSemaphoreGive( xSemaphore2);

                if( xSemaphore != NULL )
                {

                    if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
                   {
                       //parse GPS data and store it
                       sprintf(doop,"%s",msg);
                       split_GPGGA(doop, &my_location);
                       run_distances(my_location,0);
                       xSemaphoreGive( xSemaphore );
                   }
                }
				
                //sendUARTstring(2, doop, 100);
               // sendUARTstring(2, "TEST\n\r", 7);
               // sendUARTstring(6, doop, 100);
               //  sendUARTstring(DEBUG_UART, "TEST\n\r", 7);
                sendUARTstring(DEBUG_UART, doop, 100);
                toggleLED ^= 1;
				
                LEDWrite(LED1, toggleLED);
                vTaskResume(th1);
                vTaskResume(th2);

                LEDWrite(0x01, toggleLED);
            }

       //     gaugeData[0] = 2;
       //     gaugeData[1] = 0;



        }
        else
        {
            //pass errorcode to host
           // sprintf(doop,"@IMUDATA:%05d%s",0,ERRORCODE);
           // sendUARTstring(2, doop, 100);
            //sendUARTstring(6, doop, 100);

            gaugeData[0] = 8;
            //sendUARTstring(2, "TEST\n\r", 7);
        //    SysCtlReset();

            #if PING_ALIVE
                        sendUARTstring(2, "ALIVE\n\r", 8);
            #endif
            //SysCtlReset();

            /* Did not receive a notification within the expected time. Resetting. */
            //prvCheckForErrors();
        }



    }
}


void vTimerCallback1( TimerHandle_t xTimer )
{
    uint32_t poo;
     /* The TX ISR has set a bit. */
     if( xQueue1 != 0 )
     {

         // Receive a message on the created queue.  Block for 10 ticks if a
         // message is not immediately available.
         if( xQueueReceive( xQueue1, &poo, ( TickType_t ) 10 ) )
         {
             //write to LED to show GPS time-sync difference
             //LEDWrite(0x02,poo%2<<1);
             LEDWrite(LED2,poo%2<<1);
         }
     }
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
