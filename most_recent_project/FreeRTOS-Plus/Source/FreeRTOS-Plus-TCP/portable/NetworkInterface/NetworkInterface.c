// Standard includes. //
#include <stdint.h>
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
#include "NetworkBufferManagement.h"

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

static void prvEMACDeferredInterruptHandlerTask( void *pvParameters )
{
    NetworkBufferDescriptor_t *pxBufferDescriptor;
    size_t xBytesReceived;
    // Used to indicate that xSendEventStructToIPTask() is being called because
    //of an Ethernet receive event. //
    IPStackEvent_t xRxEvent;

    for( ;; )
    {
        // Wait for the Ethernet MAC interrupt to indicate that another packet
        //has been received.  The task notification is used in a similar way to a
        //counting semaphore to count Rx events, but is a lot more efficient than
        //a semaphore. //
        ulTaskNotifyTake( pdFALSE, portMAX_DELAY );

        // See how much data was received.  Here it is assumed ReceiveSize() is
        //a peripheral driver function that returns the number of bytes in the
        //received Ethernet frame. //
        //xBytesReceived = ReceiveSize();

        if( xBytesReceived > 0 )
        {
            // Allocate a network buffer descriptor that points to a buffer
            //large enough to hold the received frame.  As this is the simple
            //rather than efficient example the received data will just be copied
            //into this buffer. //
            pxBufferDescriptor = pxGetNetworkBufferWithDescriptor( xBytesReceived, 0 );

            if( pxBufferDescriptor != NULL )
            {
                // pxBufferDescriptor->pucEthernetBuffer now points to an Ethernet
                //buffer large enough to hold the received data.  Copy the
                //received data into pcNetworkBuffer->pucEthernetBuffer.  Here it
                //is assumed ReceiveData() is a peripheral driver function that
                //copies the received data into a buffer passed in as the function's
                //parameter.  Remember! While is is a simple robust technique -
                //it is not efficient.  An example that uses a zero copy technique
                //is provided further down this page. //
                ReceiveData( pxBufferDescriptor->pucEthernetBuffer );
                pxBufferDescriptor->xDataLength = xBytesReceived;

                // See if the data contained in the received Ethernet frame needs
                //to be processed.  NOTE! It is preferable to do this in
                //the interrupt service routine itself, which would remove the need
                //to unblock this task for packets that don't need processing. //
                if( eConsiderFrameForProcessing( pxBufferDescriptor->pucEthernetBuffer )
                                                                      == eProcessBuffer )
                {
                    // The event about to be sent to the TCP/IP is an Rx event. //
                    xRxEvent.eEventType = eNetworkRxEvent;

                    // pvData is used to point to the network buffer descriptor that
                    //now references the received data. //
                    xRxEvent.pvData = ( void * ) pxBufferDescriptor;

                    // Send the data to the TCP/IP stack. //
                    if( xSendEventStructToIPTask( &xRxEvent, 0 ) == pdFALSE )
                    {
                        // The buffer could not be sent to the IP task so the buffer
                        //must be released. //
                        vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );

                        // Make a call to the standard trace macro to log the
                       // occurrence. //
                        iptraceETHERNET_RX_EVENT_LOST();
                    }
                    else
                    {
                        // The message was successfully sent to the TCP/IP stack.
                        //Call the standard trace macro to log the occurrence. //
                        iptraceNETWORK_INTERFACE_RECEIVE();
                    }
                }
                else
                {
                    // The Ethernet frame can be dropped, but the Ethernet buffer
                    //must be released.
                    vReleaseNetworkBufferAndDescriptor( pxBufferDescriptor );
                }
            }
            else
            {
                // The event was lost because a network buffer was not available.
                //Call the standard trace macro to log the occurrence. //
                iptraceETHERNET_RX_EVENT_LOST();
            }
        }
    }
}

