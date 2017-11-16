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
#include "event_queue.h"


typedef struct {
    volatile uint8_t dma_pending;
} tUSBDevice;


extern tUSBDevice USBDevice;


//void SendBuffer(uint8_t *buffer);

void USBDeviceMain(void);

void USBDeviceInit(void);

#endif /* USB_DEVICE_H_ */
