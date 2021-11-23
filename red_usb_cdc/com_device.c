/*
 * com_device.c
 *
 *  Created on: 23.11.2021
 *      Author: kalan
 */


#include <stdbool.h>
#include <stdint.h>

#include "com_device.h"

#include "lenlab_protocol.h"
#include "lenlab_version.h"

#include "driverlib/usb.h"

#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"


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
    '8', 0, '.', 0, '0', 0, '.', 0, '0', 0
};

//*****************************************************************************
//
// The control interface description string.
//
//*****************************************************************************
const uint8_t g_pui8ControlInterfaceString[] =
{
    2 + (21 * 2),
    USB_DTYPE_STRING,
    'A', 0, 'C', 0, 'M', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0, 't', 0,
    'r', 0, 'o', 0, 'l', 0, ' ', 0, 'I', 0, 'n', 0, 't', 0, 'e', 0,
    'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0
};

//*****************************************************************************
//
// The configuration description string.
//
//*****************************************************************************
const uint8_t g_pui8ConfigString[] =
{
    2 + (26 * 2),
    USB_DTYPE_STRING,
    'S', 0, 'e', 0, 'l', 0, 'f', 0, ' ', 0, 'P', 0, 'o', 0, 'w', 0,
    'e', 0, 'r', 0, 'e', 0, 'd', 0, ' ', 0, 'C', 0, 'o', 0, 'n', 0,
    'f', 0, 'i', 0, 'g', 0, 'u', 0, 'r', 0, 'a', 0, 't', 0, 'i', 0,
    'o', 0, 'n', 0
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
    g_pui8ControlInterfaceString,
    g_pui8ConfigString
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_ppui8StringDescriptors) /                \
                                sizeof(uint8_t *))

//*****************************************************************************
//
// CDC device callback function prototypes.
//
//*****************************************************************************
uint32_t RxHandler(void *pvCBData, uint32_t ui32Event,
                   uint32_t ui32MsgValue, void *pvMsgData);
uint32_t TxHandler(void *pvCBData, uint32_t ui32Event,
                   uint32_t ui32MsgValue, void *pvMsgData);
uint32_t ControlHandler(void *pvCBData, uint32_t ui32Event,
                        uint32_t ui32MsgValue, void *pvMsgData);


tUSBDCDCDevice g_sCDCDevice =
    {
    //
    // The Vendor ID you have been assigned by USB-IF.
    //
    LENLAB_VID,
    //
    // The product ID you have assigned for this device.
    //
    LENLAB_PID,
    //
    // The power consumption of your device in milliamps.
    //
    0,
    //
    // The value to be passed to the host in the USB configuration descriptor’s
    // bmAttributes field.
    //
    USB_CONF_ATTR_SELF_PWR,
    //
    // A pointer to your control callback event handler.
    //
    ControlHandler,
    //
    // A value that you want passed to the control callback alongside every
    // event.
    //
    (void *) 0,
    //
    // A pointer to your receive callback event handler.
    //
    RxHandler,
    //
    // A value that you want passed to the receive callback alongside every
    // event.
    //
    (void *) 0,
    //
    // A pointer to your transmit callback event handler.
    //
    TxHandler,
    //
    // A value that you want passed to the transmit callback alongside every
    // event.
    //
    (void *) 0,
    //
    // A pointer to your string table.
    //
    g_ppui8StringDescriptors,
    //
    // The number of entries in your string table.
    //
    NUM_STRING_DESCRIPTORS
};

struct COMDevice com_device;

//*****************************************************************************
//
// Get the communication parameters in use on the UART.
//
//*****************************************************************************
static void
GetLineCoding(tLineCoding *psLineCoding)
{
    psLineCoding->ui32Rate = 9600; // bit per second
    psLineCoding->ui8Databits = 8;
    psLineCoding->ui8Parity = USB_CDC_PARITY_NONE;
    psLineCoding->ui8Stop = USB_CDC_STOP_BITS_1;
}

//*****************************************************************************
//
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
ControlHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData)
{
    //uint32_t ui32IntsOff;

    //
    // Which event are we being asked to process?
    //
    switch(ui32Event)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
            com_device.connected = true;
            /*
            g_bUSBConfigured = true;

            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);

            //
            // Tell the main loop to update the display.
            //
            ui32IntsOff = MAP_IntMasterDisable();
            g_pcStatus = "Connected";
            g_ui32Flags |= COMMAND_STATUS_UPDATE;
            if(!ui32IntsOff)
            {
                MAP_IntMasterEnable();
            }
            */
            break;

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
            com_device.connected = false;
            /*
            g_bUSBConfigured = false;
            ui32IntsOff = MAP_IntMasterDisable();
            g_pcStatus = "Disconnected";
            g_ui32Flags |= COMMAND_STATUS_UPDATE;
            if(!ui32IntsOff)
            {
                MAP_IntMasterEnable();
            }
            */
            break;

        //
        // Return the current serial communication parameters.
        //
        case USBD_CDC_EVENT_GET_LINE_CODING:
            GetLineCoding(pvMsgData);
            break;

        /*

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_LINE_CODING:
            SetLineCoding(pvMsgData);
            break;

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            SetControlLineState((uint16_t)ui32MsgValue);
            break;

        //
        // Send a break condition on the serial line.
        //
        case USBD_CDC_EVENT_SEND_BREAK:
            SendBreak(true);
            break;

        //
        // Clear the break condition on the serial line.
        //
        case USBD_CDC_EVENT_CLEAR_BREAK:
            SendBreak(false);
            break;

        */

        //
        // Ignore SUSPEND and RESUME for now.
        //
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }

    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param ui32CBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
TxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData)
{
    //
    // Which event have we been sent?
    //
    switch(ui32Event)
    {
        case USB_EVENT_TX_COMPLETE:
            //
            // Since we are using the USBBuffer, we don't need to do anything
            // here.
            //
            break;

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }
    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param ui32CBData is the client-supplied callback data value for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
RxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue, void *pvMsgData)
{
    static uint8_t buffer[64];

    //uint32_t ui32Count;

    //
    // Which event are we being sent?
    //
    switch(ui32Event)
    {
        //
        // A new packet has been received.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            uint32_t length = USBDCDCRxPacketAvailable(com_device.device);
            USBDCDCPacketRead(com_device.device, buffer, length, false);
            break;
        }

        //
        // We are being asked how much unprocessed data we have still to
        // process. We return 0 if the UART is currently idle or 1 if it is
        // in the process of transmitting something. The actual number of
        // bytes in the UART FIFO is not important here, merely whether or
        // not everything previously sent to us has been transmitted.
        //
        case USB_EVENT_DATA_REMAINING:
        {
            //
            // Get the number of bytes in the buffer and add 1 if some data
            // still has to clear the transmitter.
            //
            //ui32Count = MAP_UARTBusy(USB_UART_BASE) ? 1 : 0;
            //return(ui32Count);
            return(0);
        }

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(0);
}


void
COMDeviceInit()
{
    //
    // Set the USB stack mode to Device mode with VBUS monitoring.
    //
    USBStackModeSet(0, eUSBModeForceDevice, 0);

    //
    // Pass our device information to the USB library and place the device
    // on the bus.
    //
    com_device.device = USBDCDCInit(0, &g_sCDCDevice);
}
