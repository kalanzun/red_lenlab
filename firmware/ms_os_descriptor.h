/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#ifndef MS_OS_DESCRIPTOR_H_
#define MS_OS_DESCRIPTOR_H_

#include "usblib/usblib.h"

//
// Microsoft OS String Descriptor
//

#define USB_MS_OS_STRING_DESC_IDX 0xEE
#define USB_MS_OS_STRING_DESC_SIGNATURE { 'M', 0, 'S', 0, 'F', 0, 'T', 0, '1', 0, '0', 0, '0', 0 }

typedef struct _USB_MS_OS_STRING_DESCRIPTOR
{
    uint8_t  bLength;               // Length of the descriptor
    uint8_t  bDescriptorType;       // Descriptor type
    uint8_t  qwSignature[14];       // Signature field
    uint8_t  bMS_VendorCode;        // Vendor code
    uint8_t  bPad;                  // Pad field
} PACKED tUSB_MS_OS_STRING_DESCRIPTOR;

#define USB_MS_OS_STRING_DESCRIPTOR( vendor_code ) { \
    .bLength         = sizeof(tUSB_MS_OS_STRING_DESCRIPTOR), \
    .bDescriptorType = USB_DTYPE_STRING, \
    .qwSignature     = USB_MS_OS_STRING_DESC_SIGNATURE, \
    .bMS_VendorCode  = USB_MS_OS_VENDOR_CODE, \
    .bPad            = 0x00 \
}


//
// Microsoft Extended Compat ID OS Feature Descriptor
//

#define USB_MS_EXTENDED_COMPAT_ID_VERSION 0x0100
#define USB_MS_EXTENDED_COMPAT_ID_TYPE 0x04

#define USB_COMPATID_NONE {0}
#define USB_SUBCOMPATID_NONE {0}
#define USB_COMPATID_WINUSB "WINUSB\0"

typedef struct _MS_EXTENDED_COMPAT_ID_HEADER
{
    uint32_t dwLength;              // length of the complete descriptors set
    uint16_t bcdVersion;            // descriptor's version number (BCD: 0x0100)
    uint16_t wIndex;                // type number of extended compat descriptor (0x04)
    uint8_t  bCount;                // number of custom property sections
    uint8_t  rgbReserved[7];        // reserved
} PACKED tMS_EXTENDED_COMPAT_ID_HEADER;

#define MS_EXTENDED_COMPAT_ID_HEADER( size, count ) { \
    .dwLength       = size, \
    .bcdVersion     = USB_MS_EXTENDED_COMPAT_ID_VERSION, \
    .wIndex         = USB_MS_EXTENDED_COMPAT_ID_TYPE, \
    .bCount         = count, \
    .rgbReserved    = {0} \
}

typedef struct _MS_EXTENDED_COMPAT_ID_FUNCTION
{
    uint8_t  bFirstInterfaceNumber; // interface or function number
    uint8_t  bReserved;             // reserved
    uint8_t  compatibleID[8];       // function's compatible ID
    uint8_t  subCompatibleID[8];    // function's subcompatible ID
    uint8_t  rgbReserved[6];        // reserved
} PACKED tMS_EXTENDED_COMPAT_ID_FUNCTION;

#define MS_EXTENDED_COMPAT_ID_FUNCTION( interfaceNo, id, subID ) { \
    .bFirstInterfaceNumber = interfaceNo, \
    .bReserved             = 0x01, \
    .compatibleID          = id, \
    .subCompatibleID       = subID, \
    .rgbReserved           = {0} \
}


//
// Microsoft Extended Properties OS Feature Descriptor
//

#define USB_MS_EXTENDED_PROPERTY_VERSION 0x0100
#define USB_MS_EXTENDED_PROPERTY_TYPE 0x05

#define USB_EX_PROPERTY_REG_SZ            0x00000001  // NULL-terminated Unicode String
#define USB_EX_PROPERTY_REG_EXPAND_SZ     0x00000002  // NULL-terminated Unicode String (environment var)
#define USB_EX_PROPERTY_REG_BINARY        0x00000003  // Binary
#define USB_EX_PROPERTY_REG_DWORD_LE      0x00000004  // little-endian 32-bit integer
#define USB_EX_PROPERTY_REG_DWORD_BE      0x00000005  // big-endian 32-bit integer
#define USB_EX_PROPERTY_REG_LINK          0x00000006  // NULL-terminated Unicode String (symbolic link)
#define USB_EX_PROPERTY_REG_MULTI_SZ      0x00000007  // Multi NULL-terminated Unicode String

#define USB_EX_PROPERTY_DATA_LEN_DWORD 4

typedef struct _MS_EXTENDED_PROPERTY_HEADER
{
    uint32_t dwLength;              // length of the complete descriptors set
    uint16_t bcdVersion;            // descriptor's version number (BCD: 0x0100)
    uint16_t wIndex;                // type number of extended property desc (0x05)
    uint16_t wCount;                // number of custom property sections
} PACKED tMS_EXTENDED_PROPERTY_HEADER;

#define MS_EXTENDED_PROPERTY_HEADER( size, count ) { \
    .dwLength   = size, \
    .bcdVersion = USB_MS_EXTENDED_PROPERTY_VERSION, \
    .wIndex     = USB_MS_EXTENDED_PROPERTY_TYPE, \
    .wCount     = count \
}

// DeviceInterfaceGUID - interface GUID for WinUSB

#define USB_EX_PROPERTY_NAME_LENGTH_IFGUID 0x0028
#define USB_EX_PROPERTY_DATA_LEN_IFGUID 0x0000004E

typedef struct _MS_EXTENDED_PROPERTY_INTERFACE_GUID
{
    uint32_t dwSize;                // size of this section
    uint32_t dwPropertyDataType;    // property data format
    uint16_t wPropertyNameLength;   // property name length
    uint16_t bPropertyName[USB_EX_PROPERTY_NAME_LENGTH_IFGUID / sizeof(uint16_t)];   // property name
    uint32_t dwPropertyDataLength;  // length of property data
    uint16_t bPropertyData[USB_EX_PROPERTY_DATA_LEN_IFGUID / sizeof(uint16_t)];      // property data
} PACKED tMS_EXTENDED_PROPERTY_INTERFACE_GUID;

#define MS_EXTENDED_PROPERTY_INTERFACE_GUID( guid ) { \
    .dwSize               = sizeof( tMS_EXTENDED_PROPERTY_INTERFACE_GUID ), \
    .dwPropertyDataType   = USB_EX_PROPERTY_REG_SZ, \
    .wPropertyNameLength  = USB_EX_PROPERTY_NAME_LENGTH_IFGUID, \
    .bPropertyName        = L"DeviceInterfaceGUID", \
    .dwPropertyDataLength = USB_EX_PROPERTY_DATA_LEN_IFGUID, \
    .bPropertyData        = guid \
}

#endif /* MS_OS_DESCRIPTOR_H_ */
