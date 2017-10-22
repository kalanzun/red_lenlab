/*
 * usb_device.c
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "usblib/usb-ids.h"
#include "usb_device.h"
#include "command_handler.h"
#include "debug.h"

/*
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "usblib/usb-ids.h"
*/

//*****************************************************************************
//
// The languages supported by this device.
//
//*****************************************************************************
const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//*****************************************************************************
//
// The manufacturer string.
//
//*****************************************************************************
const uint8_t g_pui8ManufacturerString[] =
{
    (17 + 1) * 2,
    USB_DTYPE_STRING,
    'T', 0, 'e', 0, 'x', 0, 'a', 0, 's', 0, ' ', 0, 'I', 0, 'n', 0, 's', 0,
    't', 0, 'r', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 's', 0,
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
const uint8_t g_pui8ProductString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'G', 0, 'e', 0, 'n', 0, 'e', 0, 'r', 0, 'i', 0, 'c', 0, ' ', 0, 'B', 0,
    'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0,
    'e', 0
};

//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const uint8_t g_pui8SerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The data interface description string.
//
//*****************************************************************************
const uint8_t g_pui8DataInterfaceString[] =
{
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0,
    'a', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const uint8_t g_pui8ConfigString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const uint8_t * const g_ppui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SerialNumberString,
    g_pui8DataInterfaceString,
    g_pui8ConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_ppui8StringDescriptors) /                \
                                sizeof(uint8_t *))

//*****************************************************************************
//
// The bulk device initialization and customization structures. In this case,
// we are using USBBuffers between the bulk device class driver and the
// application code. The function pointers and callback data values are set
// to insert a buffer in each of the data channels, transmit and receive.
//
// With the buffer in place, the bulk channel callback is set to the relevant
// channel function and the callback data is set to point to the channel
// instance data. The buffer, in turn, has its callback set to the application
// function and the callback data set to our bulk instance structure.
//
//*****************************************************************************
uint32_t YourUSBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);
uint32_t YourUSBTransmitEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);

// USBDBulkInit does not accept a const tUSBDBulkDevice *
tUSBDBulkDevice USBBulkDevice = {
    USB_VID_TI_1CBE,
    USB_PID_BULK,
    500, // mA
    USB_CONF_ATTR_SELF_PWR,
    YourUSBReceiveEventCallback,
    &USBDevice,
    YourUSBTransmitEventCallback,
    &USBDevice,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS
};


uint32_t
YourUSBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{
    tUSBDevice *self;
    uint32_t size;
    tPacket *packet;

    //
    // Which event have we been sent?
    //
    switch(ui32Event)
    {
        //
        // Data is available from the lower layer.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            self = (tUSBDevice*) pvCBData;
            size = USBDBulkRxPacketAvailable(self->bulk_device);
            if (size > PACKET_QUEUE_PAYLOAD_SIZE) size = PACKET_QUEUE_PAYLOAD_SIZE;
            if (size) {
                packet = PacketQueueWrite(self->command_queue);
                if (packet) {
                    packet->size = size;
                    USBDBulkPacketRead(self->bulk_device, packet->payload, size, true);
                }
                else {
                    DEBUG_PRINT("Error, buffer overflow in USB receive callback\n");
                    while (1) {}; // TODO Implement Buffer Overflow Handling
                }
            }
            return size;
        }
    }

    return 0;

}


uint32_t
YourUSBTransmitEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{

    //
    // Which event have we been sent?
    //
    switch(ui32Event)
    {
        //
        // A previous transmission has completed.
        //
        case USB_EVENT_TX_COMPLETE:
        {
            /*
            if (g_ui32TxCount == 0)
            {
                USBDBulkPacketWrite(&g_sBulkDevice, data, 64, true);
                g_ui32TxCount = 1;
            }
            else
                g_ui32TxCount = 0;
            */

            return 0;
        }
    }

    return 0;
}

void
USBDeviceInit(tUSBDevice *self)
{
    self->bulk_device = &USBBulkDevice;

    //
    // Enable the GPIO peripheral used for USB, and configure the USB
    // pins.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
    {
    }

    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //
    // Set the USB stack mode to Device mode with VBUS monitoring.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    USBDBulkInit(0, self->bulk_device);

    //
    // Configure the DMA for the IN endpoint.
    //
    /*
    //ui8INDMA = USBLibDMAChannelAllocate(g_psUSBDMAInst, USB_EP_1, 64, USB_DMA_EP_TX | USB_DMA_EP_DEVICE);
    USBEndpointDMAChannel(USB0_BASE, USB_EP_1, UDMA_CHANNEL_USBEP1TX);
    uDMAChannelAttributeDisable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_ALL);
    uDMAChannelAttributeEnable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_USEBURST);
    // manual says to set burst mode, the DMA code of usblib doesn't. Both work.
    uDMAChannelControlSet(UDMA_CHANNEL_USBEP1TX, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_64);
    // FIFO byte-weise befüllen, UDMA_SIZE_32 funktioniert nicht
    // USBEndpointDMADisable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);
    // offenbar nicht nötig

    //USBLibDMAUnitSizeSet(g_psUSBDMAInst, ui8INDMA, 8);

    //USBLibDMAArbSizeSet(g_psUSBDMAInst, ui8INDMA, 16);
    */

}
