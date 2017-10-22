/*
 * usb_device.h
 *
 *  Created on: 22.10.2017
 *      Author: Christoph
 */

#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

#include "usblib/usblib.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdbulk.h"
#include "packet_queue.h"


typedef struct {
    tPacketQueue *command_queue;
    tUSBDBulkDevice *bulk_device;
} tUSBDevice;


extern tUSBDevice USBDevice;

void USBDeviceInit(tUSBDevice *self);


#endif /* USB_DEVICE_H_ */
