/*
 * ms_os_descriptor.c
 *
 *  Created on: 19.11.2017
 *      Author: Christoph
 */

#include <stdint.h>
#include <stdbool.h>
#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "ms_os_descriptor.h"
#include "debug.h"


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


#define min(a, b) (((a) < (b)) ? (a) : (b))


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
        len = min( pUSBRequest->wLength, sizeof(g_USBExtendedCompatIDDesc) );
        USBDCDSendDataEP0( 0, (uint8_t *)&g_USBExtendedCompatIDDesc, len );
        return;
    }

    if ( ( (pUSBRequest->bmRequestType == (USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR | USB_RTYPE_INTERFACE) )
        || (pUSBRequest->bmRequestType == (USB_RTYPE_DIR_IN | USB_RTYPE_VENDOR) ) ) // workaround for Renesus USB3.0 driver
      && (pUSBRequest->bRequest      == USB_MS_OS_VENDOR_CODE)
      && (pUSBRequest->wValue        == USB_WINUSB_INTERFACE_NUM)
      && (pUSBRequest->wIndex        == USB_MS_EXTENDED_PROPERTY_TYPE)
       ) {
        len = min( pUSBRequest->wLength, sizeof(g_USBExtendedPropIfGUIDDesc) );
        USBDCDSendDataEP0( 0, (uint8_t *)& g_USBExtendedPropIfGUIDDesc, len );
        return;
    }

    // STALL, if no one processes this request
    USBDCDStallEP0(0);
}
