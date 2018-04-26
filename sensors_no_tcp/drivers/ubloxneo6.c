#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils/uart_nointerrupt.h"
#include "drivers/ubloxneo6.h"
#include "main.h"

uint8_t detect_gps()
{
    return 0; //TODO
}

uint8_t init_gps(uint32_t module, uint32_t sysclock)
{
    initUART(module, 9600, sysclock);
    return 0;
}
uint8_t get_gps(uint32_t module, char* msg)
{
    //initUART(0, 57600, SYSTEM_CLOCK); //testing

    uint8_t msglen = 83;
    char i = 0;


    do
    {
        memset(msg, 0, msglen * (sizeof msg[0]) );
        getUARTline(module, msg, msglen);
        i++;
        if(i>50)
        {
            return 1; //timeout failure
        }
    }while(!strstr(msg,"$GPGGA"));
    //sendUARTstring(0, msg, msglen);
    return 0;
}
