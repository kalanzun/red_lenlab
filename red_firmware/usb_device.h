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

#ifndef USB_DEVICE_H_
#define USB_DEVICE_H_

#include "ring.h"

typedef struct USBDevice {
    volatile bool dma_pending;
    volatile bool tx_pending;
} tUSBDevice;


extern tUSBDevice usb_device;

/*
void USBDeviceSend(tUSBDevice *self, tRing *ring);

void USBDeviceSendInterleaved(tUSBDevice *self, tRing *ring0, tRing *ring1);
*/

void USBDeviceMain(tUSBDevice *self);

void USBDeviceInit(tUSBDevice *self);

#endif /* USB_DEVICE_H_ */
