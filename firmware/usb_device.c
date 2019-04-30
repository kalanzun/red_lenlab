/*
 * usb_device.c
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "usb_device.h"

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"
#include "driverlib/debug.h"

#include "usblib/usb-ids.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"

#include "lenlab_protocol.h"
#include "command_handler.h"
#include "reply_handler.h"

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
    (33 + 1) * 2,
    USB_DTYPE_STRING,
    'K', 0, 'a', 0, 'r', 0, 'l', 0, 's', 0, 'r', 0, 'u', 0, 'h', 0, 'e', 0,
    ' ', 0, 'I', 0, 'n', 0, 's', 0, 't', 0, 'i', 0, 't', 0, 'u', 0, 't', 0,
    'e', 0, ' ', 0, 'o', 0, 'f', 0, ' ', 0, 'T', 0, 'e', 0, 'c', 0, 'h', 0,
    'n', 0, 'o', 0, 'l', 0, 'o', 0, 'g', 0, 'y', 0
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
const uint8_t g_pui8ProductString[] =
{
    (16 + 1) * 2,
    USB_DTYPE_STRING,
    'L', 0, 'e', 0, 'n', 0, 'l', 0, 'a', 0, 'b', 0, ' ', 0, 'R', 0, 'e', 0,
    'd', 0, ' ', 0, 'B', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0
};

//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const uint8_t g_pui8SerialNumberString[] =
{
    (5 + 1) * 2,
    USB_DTYPE_STRING,
    '7', 0, '.', 0, '1', 0, '.', 0, '0', 0
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
tUSBDBulkDevice bulk_device = {
    LENLAB_VID,
    LENLAB_PID,
    500, // mA
    USB_CONF_ATTR_SELF_PWR,
    YourUSBReceiveEventCallback,
    0,
    YourUSBTransmitEventCallback,
    0,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS
};

uint32_t
YourUSBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{
    uint32_t size;
    tEvent *event;

    // Note: USB does not signal software disconnect. This function or the interrupt handler
    // is not called, if Lenlab on the host disappears.

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
            size = USBDBulkRxPacketAvailable(&bulk_device);
            //DEBUG_PRINT("USB_EVENT_RX_AVAILABLE %d\n", size);
            if (size > LENLAB_PACKET_HEAD_LENGTH + LENLAB_PACKET_BODY_LENGTH)
                size = LENLAB_PACKET_HEAD_LENGTH + LENLAB_PACKET_BODY_LENGTH;
            if (size) {
                if (!QueueFull(&command_handler.command_queue)) {
                    event = QueueAcquire(&command_handler.command_queue);
                    event->length = size;
                    USBDBulkPacketRead(&bulk_device, event->payload, size, true);
                    //DEBUG_PRINT("put command\n");
                    QueueWrite(&command_handler.command_queue);
                }
                else {
                    //ASSERT(0);
                    // TODO Command Queue Full, seems to happen because of DMA transfer???
                    DEBUG_PRINT("command queue full\n");
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
            //DEBUG_PRINT("USB_EVENT_TX_COMPLETE\n");

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

inline void
USBDeviceStartuDMA(tUSBDevice *self, uint8_t *payload, uint32_t length)
{
    ASSERT(!usb_device.dma_pending);
    self->dma_pending = true;
    //
    // Configure and enable DMA for the IN transfer.
    //
    //USBLibDMATransfer(g_psUSBDMAInst, ui8INDMA, pi8Data, 1024);
    USBEndpointDMAConfigSet(USB0_BASE, USB_EP_1, USB_EP_MODE_BULK | USB_EP_DEV_IN | USB_EP_DMA_MODE_1 | USB_EP_AUTO_SET);
    // does not work if moved up into the config section

    uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1TX, UDMA_MODE_BASIC, payload,
                           (void *)USBFIFOAddrGet(USB0_BASE, USB_EP_1), length);
    // works only with multiples of 16 and up to 1024


    //USBEndpointPacketCountSet(USB0_BASE, USB_EP_1,
    //                                  2048/64);
    // offenbar nicht noetig

    //
    // Start the DMA transfer.
    //
    //USBLibDMAChannelEnable(g_psUSBDMAInst, ui8INDMA);

    USBEndpointDMAEnable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);
    uDMAChannelEnable(UDMA_CHANNEL_USBEP1TX);
    // both are needed here
}

inline void
USBDeviceIntHandler(tUSBDevice *self)
{
    tEvent *event;
    tPage *page;

    if ((self->dma_pending) && (uDMAChannelModeGet(UDMA_CHANNEL_USBEP1TX) == UDMA_MODE_STOP))
    {
        //
        // Handle the DMA complete case.
        //
        self->dma_pending = false;

        event = QueueRead(&reply_handler.reply_queue);
        if (event->ring) {
            RingRelease(event->ring);
            // Free memory, if this was the last page
            if (RingEmpty(event->ring)) {
                RingFree(event->ring);
                QueueRelease(&reply_handler.reply_queue);
            }
            else {
                page = RingRead(event->ring);
                USBDeviceStartuDMA(self, (uint8_t *) page->buffer, 4 * PAGE_LENGTH);
                // because of immediate restart, it does not receive commands meanwhile
                // commands wait, till the ring is through
                // TODO command waiting during DMA transfer seems handy for a simpler state machine
                // no special memory locking while DMA transfer necessary
                // how fast is it?
            }
        }
        else {
            USBEndpointDataSend(USB0_BASE, USB_EP_1, USB_TRANS_IN); // commit shorter packages than 1024
            QueueRelease(&reply_handler.reply_queue);
            // Main will start a new DMA transfer eventually
        }
    }
    else
    {
        USB0DeviceIntHandler();
    }
}

void
USB0IntHandler()
{
    USBDeviceIntHandler(&usb_device);
}

void
USBDeviceMain(tUSBDevice *self)
{
    tEvent *event;
    tPage *page;

    if (!self->dma_pending)// && USBDBulkTxPacketAvailable(&bulk_device) == 64)
    {
        if (!QueueEmpty(&reply_handler.reply_queue)) {
            //ASSERT(!self->send_reply);
            event = QueueRead(&reply_handler.reply_queue);
            //self->send_reply = true;
            if (event->ring) {
                page = RingRead(event->ring);
                USBDeviceStartuDMA(self, (uint8_t *) page->buffer, 4 * PAGE_LENGTH);
            }
            else {
                USBDeviceStartuDMA(self, event->payload, event->length);
            }
            //ASSERT(USBDBulkPacketWrite(&bulk_device, event->payload, event->length, true));
            //QueueRelease(&reply_handler.reply_queue);
            //DEBUG_PRINT("send reply\n");
        }
    }
}

inline void
ConfigureUSBDevice(tUSBDevice *self)
{
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);
}

void
USBDeviceInit(tUSBDevice *self)
{
    self->dma_pending = false;
    ConfigureUSBDevice(self);

    //
    // Set the USB stack mode to Device mode with VBUS monitoring.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    USBDBulkInit(0, &bulk_device);

    //
    // Configure the DMA for the IN endpoint.
    //
    //ui8INDMA = USBLibDMAChannelAllocate(g_psUSBDMAInst, USB_EP_1, 64, USB_DMA_EP_TX | USB_DMA_EP_DEVICE);
    USBEndpointDMAChannel(USB0_BASE, USB_EP_1, UDMA_CHANNEL_USBEP1TX);
    uDMAChannelAttributeDisable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_ALL);
    uDMAChannelAttributeEnable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_USEBURST);
    // manual says to set burst mode, the DMA code of usblib doesn't. Both work.
    uDMAChannelControlSet(UDMA_CHANNEL_USBEP1TX, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_64);
    // FIFO byte-weise befuellen, UDMA_SIZE_32 funktioniert nicht
    // USBEndpointDMADisable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);
    // offenbar nicht noetig

    //USBLibDMAUnitSizeSet(g_psUSBDMAInst, ui8INDMA, 8);

    //USBLibDMAArbSizeSet(g_psUSBDMAInst, ui8INDMA, 16);
}
