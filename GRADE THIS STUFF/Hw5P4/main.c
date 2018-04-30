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

#define BOARD_ID_VER   "TIVA:01:1"
#define FAKE_GPS 1 //switch on this mode if GPS is being difficult
#define FAKE_GPS_DATA "$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r      " //MUST MATCH MSG_LEN!!!
#define ERRORCODE "$GPGGA,0.00,0.0,0,0.00,0,0,00,999.99,0,0,0,0,,*5C\n\r      "

//DON'T TOUCH STUFF BELOW THIS LINE//////////////////////////////////////////////////////

#define GPS_BIT     0x01
#define IMU_BIT     0x02
#define MSG_LEN     83

#define GPS_TAG  "GPSDATA"
#define IMU_TAG  "IMUDATA"
#define TAGSIZE  8

SemaphoreHandle_t xSemaphore;

QueueHandle_t xQueue1;

//Task declarations
void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);

void vTimerCallback1(TimerHandle_t xTimer);

char msg[MSG_LEN];

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
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);

    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    xSemaphore = xSemaphoreCreateMutex();

    tick_counter =0;
    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    xQueue1 = xQueueCreate( 10, sizeof(uint32_t) );

    // Create tasks
    xTaskCreate(task1, (const portCHAR *)"IMU",
                configMINIMAL_STACK_SIZE, NULL, 1, &th1);
    xTaskCreate(task2, (const portCHAR *)"GPS",
                configMINIMAL_STACK_SIZE, NULL, 1, &th2);
    xTaskCreate(task3, (const portCHAR *)"COMM",
                configMINIMAL_STACK_SIZE, NULL, 1, &th3);

    xTimer = xTimerCreate("Timer0", 250, pdTRUE, ( void * ) 0, vTimerCallback1);

    if( xTimerStart( xTimer, 0 ) != pdPASS )
    {
        //The timer could not be set into the Active state.
    }

    vTaskStartScheduler();
    return 0;
}

// Flash the LEDs on the launchpad
void task1(void *pvParameters)
{
    initi2c(2, SYSTEM_CLOCK);
    initMPU9250(2,MPU9250_ADDRESS_1);
    assert_mag(2,AK8963_ADDRESS); //startup tests
    assert_MPU(2,MPU9250_ADDRESS_1);

    for (;;)
    {
        if( xSemaphore != NULL )
        {
            if( xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
                get_sensors(2,MPU9250_ADDRESS_1,&imu_ptr);
                //xTaskNotify(th3, IMU_BIT, eSetBits);
                xSemaphoreGive( xSemaphore );
                vTaskSuspend(th1);
            }
        }
    }
}

// Flash the LEDs on the launchpad
void task2(void *pvParameters)
{
    init_gps(4,SYSTEM_CLOCK);
    for(;;)
    {
#if FAKE_GPS
        memcpy(msg, FAKE_GPS_DATA,MSG_LEN);
        vTaskDelay(1000);
#else
        get_gps(4,msg);
#endif

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
void task3(void *pvParameters)
{
    initUART(0, 57600, SYSTEM_CLOCK,UART_CONFIG_PAR_NONE);
    initUART(6, 9600, SYSTEM_CLOCK,UART_CONFIG_PAR_EVEN);

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
            memset(doop,' ',sizeof(doop));
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
                       //pass data and state information to host
                       sprintf(doop,"@%s:%05d%s",BOARD_ID_VER,imu_ptr.z_accel,msg);
                       xSemaphoreGive( xSemaphore );
                   }
                }
                sendUARTstring(0, doop, 100);
                sendUARTstring(6, doop, 100);

                toggleLED ^= 1;
                LEDWrite(0x01, toggleLED);
                vTaskResume(th1);
                vTaskResume(th2);
            }
        }
        else
        {
            //pass errorcode to host
            sprintf(doop,"@IMUDATA:%05d%s",0,ERRORCODE);
            sendUARTstring(0, doop, 100);
            sendUARTstring(6, doop, 100);
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
             LEDWrite(0x02,poo%2<<1);
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
