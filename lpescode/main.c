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

//TI compiler stuff
#include "string.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"

SemaphoreHandle_t xSemaphore;

QueueHandle_t xQueue1;

//Task declarations
void task_imu(void *pvParameters);
void task_gps(void *pvParameters);
void task_comm(void *pvParameters);

void vTimerCallback1(TimerHandle_t xTimer);

char msg[MSG_LEN];

gps_raw_t my_location;

imu_raw_t imu_ptr;

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

    xSemaphore = xSemaphoreCreateMutex();

    tick_counter =0;
    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    xQueue1 = xQueueCreate( 10, sizeof(uint32_t) );

    // Create tasks
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

 //   sprintf(temp_buffer, "\r\nDebug UART initialized.\r\n");

  //  sendUARTstring(DEBUG_UART, temp_buffer, strlen(temp_buffer) );
    return 0;
}

// Flash the LEDs on the launchpad
void task_imu(void *pvParameters)
{
    //initi2c(2, SYSTEM_CLOCK);
    //initMPU9250(2,MPU9250_ADDRESS_1);
   // assert_mag(2,AK8963_ADDRESS); //startup tests
   // assert_MPU(2,MPU9250_ADDRESS_1);

    for (;;)
    {
        if( xSemaphore != NULL )
        {
            if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
               // get_sensors(2,MPU9250_ADDRESS_1,&imu_ptr);
                //xTaskNotify(th3, IMU_BIT, eSetBits);
                xSemaphoreGive( xSemaphore );
                vTaskSuspend(th1);
            }
        }
    }
}

// Flash the LEDs on the launchpad
void task_gps(void *pvParameters)
{
    init_gps(1,SYSTEM_CLOCK);
    for(;;)
    {
#if FAKE_GPS
        memcpy(msg, FAKE_GPS_DATA,MSG_LEN);
        vTaskDelay(1000);
#else
        get_gps(1,msg);
#endif
        msg[82]='\0';
        tick_counter++;

        if( xQueue1 != 0 )
        {
            /* Send an unsigned long.  Wait for 10 ticks for space to become
            available if necessary. */
            if( xQueueSend( xQueue1,
                           ( void * ) &tick_counter,
                           ( TickType_t ) 10 ) != pdPASS )
            {
                /* Failed to post the message, even after 10 ticks. */
            }
        }

        xTaskNotify(th3, GPS_BIT, eSetBits);
        vTaskSuspend(th2);
    }
}

//communication task, sends data over uart to master device using EVEN parity, and to terminal using no parity
void task_comm(void *pvParameters)
{
    initUART(2, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);
    //initUART(6, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_EVEN);
    initUART(DEBUG_UART, 115200, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);

    const TickType_t xMaxBlockTime = 5000;
    BaseType_t xResult;
    uint32_t ulNotifiedValue;
    char toggleLED = 0;
    char doop[100];
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
                vTaskResume(th1);
            }

            if( ( ulNotifiedValue & GPS_BIT ) != 0 )
            {

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
            }
        }
        else
        {
            //pass errorcode to host
           // sprintf(doop,"@IMUDATA:%05d%s",0,ERRORCODE);
           // sendUARTstring(2, doop, 100);
            //sendUARTstring(6, doop, 100);

            //sendUARTstring(2, "TEST\n\r", 7);
            SysCtlReset();
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
