//adapted from TI TivaWare User Guide, page 193 by Zach Farmer
#include <stdint.h>
#include <stdbool.h>
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

//HAL includes
#include "eth.h"

#define SYSTEM_CLOCK    120000000U

//*****************************************************************************
//
// This example demonstrates the use of the Ethernet Controller.
//
//*****************************************************************************
int main(void)
{
    uint32_t output_clock_rate_hz = SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);

    initEthernet(output_clock_rate_hz);
    //
    // Application main loop continues....
    //
    uint64_t i = 0;
    uint8_t poo[49] = "What we've got here is a failure to communicate.";
    //for(i=0;i<32;i++)
    //{
   //     poo[i] = 0xFA;
   //a }

    while(1)
    {
        for(i=0;i<20000000UL;i++);
        PacketTransmit(poo, 49);

    }
}
