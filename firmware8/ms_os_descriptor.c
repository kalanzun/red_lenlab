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

#include "ms_os_descriptor.h"

#include "usblib/device/usbdevice.h"


#define USB_WINUSB_INTERFACE_NUM          0x00
#define USB_MS_OS_VENDOR_CODE             0x23

#define USB_TI_USB_DEV_BULK_IF_GUID       L"{0a3bd048-863e-4b88-8a49-e57eb4e2c67c}"
#define USB_TI_USB_DFU_IF_GUID            L"{fd9372d1-ceb9-442a-8883-51c34a7bbba6}"


// Microsoft OS String Descriptor

tUSB_MS_OS_STRING_DESCRIPTOR g_USBMsftOSStringDesc = USB_MS_OS_STRING_DESCRIPTOR( USB_MS_OS_VENDOR_CODE );


// MS Extended Compat ID OS Feature Descriptor

#define MS_EXTENDED_COMPAT_FUNCTION_NUM   1

typedef struct _MS_EXTENDED_COMPAT_ID_DESCRIPTOR
{
    tMS_EXTENDED_COMPAT_ID_HEADER   header;
    tMS_EXTENDED_COMPAT_ID_FUNCTION function[ MS_EXTENDED_COMPAT_FUNCTION_NUM ];
} PACKED tMS_EXTENDED_COMPAT_ID_DESCRIPTOR;

static tMS_EXTENDED_COMPAT_ID_DESCRIPTOR g_USBExtendedCompatIDDesc =
{
    .header   = MS_EXTENDED_COMPAT_ID_HEADER( sizeof( tMS_EXTENDED_COMPAT_ID_DESCRIPTOR ), MS_EXTENDED_COMPAT_FUNCTION_NUM ),
    .function = { MS_EXTENDED_COMPAT_ID_FUNCTION( USB_WINUSB_INTERFACE_NUM, USB_COMPATID_WINUSB, USB_SUBCOMPATID_NONE )
    }
};


// MS Extended Property OS Feature Descriptor

typedef struct _MS_EXTENDED_PROPERTY_DESCRIPTOR
{
    tMS_EXTENDED_PROPERTY_HEADER         header;
    tMS_EXTENDED_PROPERTY_INTERFACE_GUID property0;
} PACKED tMS_EXTENDED_PROPERTY_DESCRIPTOR;

static tMS_EXTENDED_PROPERTY_DESCRIPTOR g_USBExtendedPropIfGUIDDesc =
{
    .header    = MS_EXTENDED_PROPERTY_HEADER( sizeof( tMS_EXTENDED_PROPERTY_DESCRIPTOR ), 0x0001 ),
    .property0 = MS_EXTENDED_PROPERTY_INTERFACE_GUID( USB_TI_USB_DEV_BULK_IF_GUID )
};


bool
MSOSDescriptorHandleStringRequest(uint8_t **ppui8EP0Data, volatile uint32_t *pui32EP0DataRemain, uint16_t ui16Lang, uint16_t ui16Index)
{
    if (ui16Index == USB_MS_OS_STRING_DESC_IDX)
    {
        //
        // Return the externally specified configuration descriptor.
        //
        *ppui8EP0Data = (uint8_t *) &g_USBMsftOSStringDesc;

        //
        // The total size of a string descriptor is in byte 0.
        //
        *pui32EP0DataRemain = sizeof( tUSB_MS_OS_STRING_DESCRIPTOR );

        return true;
    }

    return false;
}


void
MSOSDescriptorHandleVendorRequest(void *pvBulkDevice, tUSBRequest *pUSBRequest)
{
    uint32_t len;

    if ( (pUSBRequest->bmRequestType == (USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR) )
      && (pUSBRequest->bRequest      == USB_MS_OS_VENDOR_CODE)
//      && (pUSBRequest->wValue        == USB_WINUSB_INTERFACE_NUM)  // should be ignored
      && (pUSBRequest->wIndex        == USB_MS_EXTENDED_COMPAT_ID_TYPE)
       ) {
        len = pUSBRequest->wLength < sizeof(g_USBExtendedCompatIDDesc) ? pUSBRequest->wLength : sizeof(g_USBExtendedCompatIDDesc);
        USBDCDSendDataEP0( 0, (uint8_t *)&g_USBExtendedCompatIDDesc, len );
    }

    if ( ( (pUSBRequest->bmRequestType == (USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR | USB_RTYPE_INTERFACE) )
        || (pUSBRequest->bmRequestType == (USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR) ) ) // workaround for Renesus USB3.0 driver
      && (pUSBRequest->bRequest      == USB_MS_OS_VENDOR_CODE)
      && (pUSBRequest->wValue        == USB_WINUSB_INTERFACE_NUM)
      && (pUSBRequest->wIndex        == USB_MS_EXTENDED_PROPERTY_TYPE)
       ) {
        len = pUSBRequest->wLength < sizeof(g_USBExtendedPropIfGUIDDesc) ? pUSBRequest->wLength : sizeof(g_USBExtendedPropIfGUIDDesc);
        USBDCDSendDataEP0( 0, (uint8_t *)& g_USBExtendedPropIfGUIDDesc, len );
    }
}
