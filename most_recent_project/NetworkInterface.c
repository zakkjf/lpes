// Standard includes. //
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
// FreeRTOS includes. //
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stream_buffer.h"

// Hardware abstraction. //
//#include "FreeRTOS_IO.h"

// FreeRTOS+TCP includes. //
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_IP.h"
#include "NetworkBufferManagement.h"

#include "driverlib/emac.h"
#include "inc/hw_memmap.h"
// Driver includes. //
#include "utils/eth.h"

// Demo includes. //
#include "NetworkInterface.h"

BaseType_t xNetworkInterfaceInitialise( void )
{
    BaseType_t xReturn;

    /*
     * Perform the hardware specific network initialisation here.  Typically
     * that will involve using the Ethernet driver library to initialise the
     * Ethernet (or other network) hardware, initialise DMA descriptors, and
     * perform a PHY auto-negotiation to obtain a network link.
     *
     * This example assumes InitialiseNetwork() is an Ethernet peripheral driver
     * library function that returns 0 if the initialisation fails.
     */
    if(initEthernet(SYSTEM_CLOCK)==0)
    {
        xReturn = pdPASS;
    }
    else
    {
        xReturn = pdFAIL;
    }

    return xReturn;
}


BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxDescriptor,
                                    BaseType_t xReleaseAfterSend )
{
    // Simple network interfaces (as opposed to more efficient zero copy network
    //interfaces) just use Ethernet peripheral driver library functions to copy
    //data from the FreeRTOS+TCP buffer into the peripheral driver's own buffer.
    //This example assumes SendData() is a peripheral driver library function that
    //takes a pointer to the start of the data to be sent and the length of the
    //data to be sent as two separate parameters.  The start of the data is located
    //by pxDescriptor->pucEthernetBuffer.  The length of the data is located
    //by pxDescriptor->xDataLength. //
    //SendData( pxDescriptor->pucBuffer, pxDescriptor->xDataLength );

    PacketTransmit(pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength);
    // Call the standard trace macro to log the send event. //
    iptraceNETWORK_INTERFACE_TRANSMIT();

    if( xReleaseAfterSend != pdFALSE )
    {
        // It is assumed SendData() copies the data out of the FreeRTOS+TCP Ethernet
        //buffer.  The Ethernet buffer is therefore no longer needed, and must be
        //freed for re-use. //
        vReleaseNetworkBufferAndDescriptor( pxDescriptor );
    }

    return pdTRUE;
}

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{

}

BaseType_t xApplicationDNSQueryHook( const char *pcName )
{
    return pdTRUE;
}

void vLoggingPrintf( const char *pcFormatString, ... )
{

}

// The deferred interrupt handler is a standard RTOS task.  FreeRTOS's centralised
//deferred interrupt handling capabilities can also be used. //
//

void EMACInterruptHandler(void)
{

    uint32_t ui32Temp;
    //
    // Read and Clear the interrupt.
    //
    ui32Temp = EMACIntStatus(EMAC0_BASE, true);
    EMACIntClear(EMAC0_BASE, ui32Temp);
    //
    // Check to see if an RX Interrupt has occurred.
    //
    if(ui32Temp & EMAC_INT_RECEIVE)
    {
        NetworkBufferDescriptor_t *pxBufferDescriptor;
        size_t xBytesReceived = 1500;
        IPStackEvent_t xRxEvent;

        pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( xBytesReceived, 0 );

        if( pxBufferDescriptor != NULL )
        {
            xBytesReceived = ProcessReceivedPacket(pxBufferDescriptor->pucEthernetBuffer);
            pxBufferDescriptor->xDataLength = xBytesReceived;

            if( eConsiderFrameForProcessing( pxBufferDescriptor->pucEthernetBuffer ) == eProcessBuffer )
            {
                xRxEvent.eEventType = eNetworkRxEvent;

                xRxEvent.pvData = ( void * ) pxBufferDescriptor;

                if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
                {
                    vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );

                    iptraceETHERNET_RX_EVENT_LOST();
                }
                else
                {
                    iptraceNETWORK_INTERFACE_RECEIVE();
                }
            }
            else
            {
                // The Ethernet frame can be dropped, but the Ethernet buffer
                //must be released.
                vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
            }

            releaseHWBuffer();
        }
        else
        {
            // The event was lost because a network buffer was not available.
            //Call the standard trace macro to log the occurrence. //
            iptraceETHERNET_RX_EVENT_LOST();
        }
    }
}

