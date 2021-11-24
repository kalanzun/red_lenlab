/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include "usb_device.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"

#include "usblib/usb-ids.h"
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"

#include "lenlab_protocol.h"


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


struct USBDevice {
    volatile bool dma_pending;
    volatile bool tx_pending;
};


static struct USBDevice usb_device;


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
static uint32_t YourUSBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);
static uint32_t YourUSBTransmitEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);


static tUSBDBulkDevice bulk_device = {
    LENLAB_VID,
    LENLAB_PID,
    0, // mA
    USB_CONF_ATTR_SELF_PWR,
    YourUSBReceiveEventCallback,
    0,
    YourUSBTransmitEventCallback,
    0,
    g_ppui8StringDescriptors,
    NUM_STRING_DESCRIPTORS
};


static uint32_t
YourUSBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{
    uint32_t size;
    static uint8_t buffer[64];

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
            size = USBDBulkRxPacketAvailable(&bulk_device);
            USBDBulkPacketRead(&bulk_device, buffer, size, true);
            return size;
    }

    return 0;
}

static uint32_t
YourUSBTransmitEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{
    switch(ui32Event)
    {
        case USB_EVENT_TX_COMPLETE:
        {
            usb_device.tx_pending = false;

            return 0;
        }
    }

    return 0;
}

static inline void
USBDeviceuDMAIntHandler(void)
{
    //
    // Handle the DMA complete case.
    //
    usb_device.dma_pending = false;
    USBEndpointDMADisable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);
}

void
USB0IntHandler(void)
{
    if (usb_device.dma_pending && uDMAChannelModeGet(UDMA_CHANNEL_USBEP1TX) == UDMA_MODE_STOP) {
        // DMA interrupt
        USBDeviceuDMAIntHandler();
    }
    else {
        // pass on to usblib
        USB0DeviceIntHandler();
    }
}

void
USBDeviceMain(void)
{
    static uint8_t buffer[1024];
    static uint32_t length = 1024;

    //USBDBulkTxPacketAvailable(&bulk_device); // it does not "see" a DMA transfer

    return;

    if (!usb_device.tx_pending && !usb_device.dma_pending) {
        //
        // Configure the address and size of the data to transfer.
        //
        uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1TX, UDMA_MODE_BASIC, buffer, (void *) USBFIFOAddrGet(USB0_BASE, USB_EP_1), length);
        // works only with multiples of 16 and up to 1024

        USBEndpointDMAConfigSet(USB0_BASE, USB_EP_1, USB_EP_MODE_BULK | USB_EP_DEV_IN | USB_EP_DMA_MODE_1 | USB_EP_AUTO_SET);

        //
        // Start the transfer.
        //
        IntDisable(INT_USB0);

        usb_device.dma_pending = true;
        USBEndpointDMAEnable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);
        uDMAChannelEnable(UDMA_CHANNEL_USBEP1TX);

        IntEnable(INT_USB0);
    }
}

void
USBDeviceInit()
{
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //
    // Set the USB stack mode to ForceDevice mode without VBUS monitoring.
    //
    // I think the Launchpad does not connect the VBUS pin,
    // so Device mode with VBUS monitoring does not work.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    USBDBulkInit(0, &bulk_device);

    //
    // Map the uDMA channel to the given endpoint.
    //
    //MAP_USBEndpointDMAChannel(psUSBDMAInst->ui32Base, ui8Endpoint,
    //                          ui32Channel);
    USBEndpointDMAChannel(USB0_BASE, USB_EP_1, UDMA_CHANNEL_USBEP1TX);

    //
    // Clear out the attributes on this channel.
    //
    //MAP_uDMAChannelAttributeDisable(ui32Channel, UDMA_ATTR_ALL);
    uDMAChannelAttributeDisable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_ALL);

    //
    // Configure the uDMA channel for the pipe
    //
    //MAP_uDMAChannelControlSet(ui32Channel,
    //                          psUSBDMAInst->pui32Config[ui32Channel]);
    uDMAChannelControlSet(UDMA_CHANNEL_USBEP1TX, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_64);

    //MAP_USBEndpointDMADisable(psUSBDMAInst->ui32Base, ui8Endpoint,
    //                          USB_EP_DEV_IN);
    USBEndpointDMADisable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN); // for now
}
