/*
 * usb_device.h
 *
 */

#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

#include "ring.h"

typedef struct USBDevice {
    volatile bool dma_pending;
} tUSBDevice;


extern tUSBDevice usb_device;

/*
void USBDeviceSend(tUSBDevice *self, tRing *ring);

void USBDeviceSendInterleaved(tUSBDevice *self, tRing *ring0, tRing *ring1);
*/

void USBDeviceMain(tUSBDevice *self);

void USBDeviceInit(tUSBDevice *self);

#endif /* USB_DEVICE_H_ */
