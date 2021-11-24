/*
 * com_device.h
 *
 *  Created on: 23.11.2021
 *      Author: kalan
 */

#ifndef COM_DEVICE_H_
#define COM_DEVICE_H_


struct COMDevice {
    void *device;
    volatile bool connected;

    uint8_t *buffer;
    uint32_t length;
};


void COMDeviceInit(void);

void COMDeviceMain(void);


#endif /* COM_DEVICE_H_ */
