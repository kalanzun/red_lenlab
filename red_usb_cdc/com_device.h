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
};


extern struct COMDevice com_device;


void COMDeviceInit();


#endif /* COM_DEVICE_H_ */
