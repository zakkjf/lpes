#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils/uart_nointerrupt.h"
#include "drivers/ubloxneo6.h"
#include "driverlib/uart.h"
#include "main.h"

uint8_t detect_gps()
{
    return 0; //TODO
}

uint8_t init_gps(uint32_t module, uint32_t sysclock)
{
    initUART(module, 9600, sysclock,UART_CONFIG_PAR_NONE);
    return 0;
}
uint8_t get_gps(uint32_t module, char* msg)
{
    //initUART(0, 57600, SYSTEM_CLOCK); //testing

    uint8_t msglen = 83;
    char i = 0;


    do
    {
       // memset(msg, 0, msglen * (sizeof msg[0]) );
        getUARTline(module, msg, msglen);
        i++;
        if(i>50)
        {
            //memset(msg, 0, msglen * (sizeof msg[0]) );
            return 1; //timeout failure
        }
    //sendUARTstring(2, msg, msglen);
    }while(NULL==strstr(msg,"$GNGGA"));
    //
    return 0;
}
