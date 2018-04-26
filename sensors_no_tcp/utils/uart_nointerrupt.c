#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

static const uint32_t g_ui32UARTPeriph[8] =
{
    SYSCTL_PERIPH_UART0,
    SYSCTL_PERIPH_UART1,
    SYSCTL_PERIPH_UART2,
    SYSCTL_PERIPH_UART3,
    SYSCTL_PERIPH_UART4,
    SYSCTL_PERIPH_UART5,
    SYSCTL_PERIPH_UART6,
    SYSCTL_PERIPH_UART7
};

static const uint32_t g_ui32UARTBase[8] =
{
     UART0_BASE,
     UART1_BASE,
     UART2_BASE,
     UART3_BASE,
     UART4_BASE,
     UART5_BASE,
     UART6_BASE,
     UART7_BASE
};


int8_t initUART(uint8_t portNum, uint32_t clock, uint32_t sysclock)
{
    if (portNum > 7)
    {
        //invalid port number
        return 1;
    }
    //get the chosen uart module

    uint32_t baseaddr = g_ui32UARTBase[portNum];
    uint32_t periph = g_ui32UARTPeriph[portNum];

    //
    // Enable the UART module.
    //
    SysCtlPeripheralEnable(periph);
    //
    // Wait for the UART module to be ready.
    //
    while(!SysCtlPeripheralReady(periph));

    // Initialize the UART. Set the baud rate, number of data bits, turn off
    // parity, number of stop bits, and stick mode. The UART is enabled by the
    // function call.
    //
    UARTConfigSetExpClk(baseaddr, sysclock, clock,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    UART_CONFIG_PAR_NONE));

    return 0;
}

int8_t getUARTstring_block(uint8_t portNum, char* str, uint32_t len)
{
    //
    // Check for characters. Spin here until a character is placed
    // into the receive FIFO.
    //
    uint32_t i;
    for(i=0;i<len;i++)
    {
        while(!UARTCharsAvail(g_ui32UARTBase[portNum]));
        (*(str+i) = UARTCharGetNonBlocking(g_ui32UARTBase[portNum]));
    }
    return 0;
}

int8_t wait_for_char_then_pull_UART(uint8_t portNum, char token, char* str, uint32_t len)
{
    //
    // Check for a characters Spin here until a character is detected, then pull the next len of characters into str
    //
    int timeout = 0;
    while(timeout<len)
    {
        timeout++;

        while(!UARTCharsAvail(g_ui32UARTBase[portNum]));
        if(token == UARTCharGetNonBlocking(g_ui32UARTBase[portNum]))
        {
            uint32_t i;
            for(i=0;i<len;i++)
            {
               while(!UARTCharsAvail(g_ui32UARTBase[portNum]));
               (*(str+i) = UARTCharGetNonBlocking(g_ui32UARTBase[portNum]));
               if(*(str+i)==0xA5)
               {
                   return 0;
               }
            }

            return 0;
        }

    }
    return 1;
}


int8_t getUARTstring_nonblock(uint8_t portNum, char* str, uint32_t len)
{
    uint32_t i;
    for(i=0;i<len;i++)
    {
        *(str+i) = UARTCharGetNonBlocking(g_ui32UARTBase[portNum]);
    }
    return 0;
}

int8_t sendUARTstring(uint8_t portNum, char* str, uint32_t len)
{
    //
    // Put a character in the output buffer.
    //
    uint32_t i;
    for(i=0;i<len;i++)
    {
            if(*(str+i)!=0xA5)
            {
                UARTCharPut(g_ui32UARTBase[portNum],*(str+i));
            }
    }
    return 0;
}

int8_t sendUARTline(uint8_t portNum, char* str)
{
    //
    // Put a character in the output buffer.
    //
    uint32_t i;
    for(i=0;i<100;i++)//max length is 100
    {
        UARTCharPut(g_ui32UARTBase[portNum],*(str+i));
        if(*(str+i)=='\n')
        {
            return i;
        }
    }
    return 0;
}

//takes a char array of max size 82 and returns the next line
int8_t getUARTline(uint8_t portNum, char* str, uint32_t maxlen)
{
    //
    // Check for characters. Spin here until a character is placed
    // into the receive FIFO.
    //

    //if(maxlen>82)
    //{
     //   return -1; //too long
    //}
    uint32_t i;
    for(i=0;i<maxlen-1;i++)
    {
        while(!UARTCharsAvail(g_ui32UARTBase[portNum]));
        *(str+i) = UARTCharGetNonBlocking(g_ui32UARTBase[portNum]);
        if(*(str+i)== 0x0A)
        {
            *(str+i+1)= '\0';
            return i+1;
        }
    }
    return 0;
}

int8_t disableUART(uint8_t portNum)
{
    //
    // Disable the givenUART.
    //
    UARTDisable(g_ui32UARTBase[portNum]);
    return 0;
}


