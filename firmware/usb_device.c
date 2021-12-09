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

#include "lenlab_version.h"

#include "command_handler.h"
#include "reply_handler.h"
#include "message.h"


static const uint8_t LangDescriptor[] = {
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};


static const uint8_t ManufacturerString[] = {
    (33 + 1) * 2,
    USB_DTYPE_STRING,
    'K', 0, 'a', 0, 'r', 0, 'l', 0, 's', 0, 'r', 0, 'u', 0, 'h', 0, 'e', 0,
    ' ', 0, 'I', 0, 'n', 0, 's', 0, 't', 0, 'i', 0, 't', 0, 'u', 0, 't', 0,
    'e', 0, ' ', 0, 'o', 0, 'f', 0, ' ', 0, 'T', 0, 'e', 0, 'c', 0, 'h', 0,
    'n', 0, 'o', 0, 'l', 0, 'o', 0, 'g', 0, 'y', 0
};


static const uint8_t ProductString[] = {
    (16 + 1) * 2,
    USB_DTYPE_STRING,
    'L', 0, 'e', 0, 'n', 0, 'l', 0, 'a', 0, 'b', 0, ' ', 0, 'R', 0, 'e', 0,
    'd', 0, ' ', 0, 'B', 0, 'o', 0, 'a', 0, 'r', 0, 'd', 0
};


static const uint8_t SerialNumberString[] = {
    (3 + 1) * 2,
    USB_DTYPE_STRING,
    '0' + LENLAB_MAJOR, 0, '.', 0, '0' + LENLAB_MINOR, 0
};


static const uint8_t DataInterfaceString[] = {
    (19 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0,
    'a', 0, 'c', 0, 'e', 0
};


static const uint8_t ConfigString[] = {
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'B', 0, 'u', 0, 'l', 0, 'k', 0, ' ', 0, 'D', 0, 'a', 0, 't', 0,
    'a', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 'f', 0, 'i', 0, 'g', 0,
    'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0, 'o', 0, 'n', 0
};


static const uint8_t * const StringDescriptors[] = {
    LangDescriptor,
    ManufacturerString,
    ProductString,
    SerialNumberString,
    DataInterfaceString,
    ConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(StringDescriptors) / sizeof(uint8_t *))


static uint32_t RxEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);
static uint32_t TxEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);


struct USBDevice usb_device = {
    .bulk_device = {
        LENLAB_VID,
        LENLAB_PID,
        0, // mA
        USB_CONF_ATTR_SELF_PWR,
        RxEventCallback,
        0,
        TxEventCallback,
        0,
        StringDescriptors,
        NUM_STRING_DESCRIPTORS
    },
};


static uint32_t
RxEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{
    struct Message *command;
    uint32_t size;

    // USB does not signal software disconnect. This function or the interrupt handler
    // is not called, if Lenlab on the host disappears.

    switch(ui32Event)
    {
        case USB_EVENT_RX_AVAILABLE:
            if (!command_queue.has_space) return 0;

            ++usb_device.rx_available_event;

            command = (struct Message *) RingAcquire(&command_queue);
            size = USBDBulkPacketRead(&usb_device, (uint8_t *) command, command_queue.element_size, false);
            ASSERT(size); // the event is USB_EVENT_RX_AVAILABLE
            // TODO implement setSize for commands
            RingWrite(&command_queue);

            return size;

        case USB_EVENT_CONNECTED:
            // the usbdbulk driver just changed the settings and removed the USB_EP_AUTO_SET bit
            USBEndpointDMAConfigSet(USB0_BASE, USB_EP_1, USB_EP_DEV_IN | USB_EP_DMA_MODE_1 | USB_EP_AUTO_SET);

            return 0;
    }

    return 0;
}


static uint32_t
TxEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData)
{
    switch(ui32Event)
    {
        case USB_EVENT_TX_COMPLETE:
            ASSERT(usb_device.tx_pending);
            usb_device.tx_pending = false;
            ++usb_device.tx_complete_event;

            return 0;
    }

    return 0;
}


static void
USBDeviceStartuDMA(uint8_t *data, uint32_t size)
{
    ASSERT(size == 1024); // automatic sending might not work for smaller packets (USB_EP_AUTO_SET)

    ASSERT(size % 16 == 0);
    ASSERT(size <= 1024);

    // Configure the address and size of the data to transfer.
    uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1TX, UDMA_MODE_BASIC, data, (void *) USBFIFOAddrGet(USB0_BASE, USB_EP_1), size);

    // Start the transfer.
    IntDisable(INT_USB0);

    usb_device.dma_pending = true;
    ++usb_device.dma_pending_event;
    usb_device.tx_pending = true;
    ++usb_device.tx_pending_event;

    USBEndpointDMAEnable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);
    uDMAChannelEnable(UDMA_CHANNEL_USBEP1TX);

    IntEnable(INT_USB0);
}


static void
USBDeviceStartTx(uint8_t *data, uint32_t size)
{
    ASSERT(size < 64); // do not trigger automatic sending with USB_EP_AUTO_SET for size == 64

    usb_device.tx_pending = true;
    ++usb_device.tx_pending_event;

    size = USBDBulkPacketWrite(&usb_device, data, size, true);
    // we tell it explicitly to send the packet (true)
    // for size == 64, this would crash with the automatic sending due to USB_EP_AUTO_SET
    ASSERT(size); // we did locking with tx_pending and dma_pending
}


void
USB0IntHandler(void)
{
    uint8_t *payload;

    if (usb_device.dma_pending && uDMAChannelModeGet(UDMA_CHANNEL_USBEP1TX) == UDMA_MODE_STOP) {
        // Handle the DMA complete case.
        usb_device.dma_pending = false;
        ++usb_device.dma_complete_event;
        USBEndpointDMADisable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN);

        RingRelease(usb_device.page_queue);
        if (usb_device.page_queue->has_content) {
            payload = RingRead(usb_device.page_queue);
            USBDeviceStartuDMA(payload, usb_device.page_queue->element_size);
        }
        else {
            RingRelease(&reply_queue);
        }
    }
    else {
        // Pass on to usblib.
        USB0DeviceIntHandler();
    }
}


void
USBDeviceMain(void)
{
    struct Message *reply;
    uint8_t *payload;

#ifdef DEBUG
    uint32_t size;
    uint32_t flags = USB_EP_DEV_IN;
#endif

    //USBDBulkTxPacketAvailable(&bulk_device); // it does not "see" a DMA transfer

    if (!usb_device.tx_pending && !usb_device.dma_pending) {

        if (reply_queue.has_content) {

#ifdef DEBUG
            // is our configuration still good?
            USBDevEndpointConfigGet(USB0_BASE, USB_EP_1, &size, &flags);
            ASSERT(flags & USB_EP_AUTO_SET);
#endif

            reply = (struct Message *) RingRead(&reply_queue);
            if (reply->head.type == PageQueue) {
                usb_device.page_queue = getPageQueue(reply);
                payload = RingRead(usb_device.page_queue);
                USBDeviceStartuDMA(payload, usb_device.page_queue->element_size);
            }
            else {
                USBDeviceStartTx((uint8_t *) reply, reply->size);
                RingRelease(&reply_queue);
            }
        }
    }
}


void
USBDeviceInit(void)
{
    // Configure the pins.
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Set the USB stack mode to ForceDevice mode without VBUS monitoring.
    //
    // The Launchpad does not connect the VBUS pin,
    // so Device mode with VBUS monitoring does not work.
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    // Pass our device information to the USB library and place the device
    // on the bus.
    USBDBulkInit(0, &usb_device.bulk_device);

    // Map the uDMA channel to the given endpoint.
    USBEndpointDMAChannel(USB0_BASE, USB_EP_1, UDMA_CHANNEL_USBEP1TX);

    // Clear out the attributes on this channel.
    uDMAChannelAttributeDisable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_ALL);

    // Enable uDMA burst mode.
    uDMAChannelAttributeEnable(UDMA_CHANNEL_USBEP1TX, UDMA_ATTR_USEBURST);

    // Configure the uDMA channel for the USB TX FIFO
    uDMAChannelControlSet(UDMA_CHANNEL_USBEP1TX, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_64);
    USBEndpointDMADisable(USB0_BASE, USB_EP_1, USB_EP_DEV_IN); // for now

    // Assumptions
    ASSERT(command_queue.element_size == 64);
    ASSERT(reply_queue.element_size == 64);
}
