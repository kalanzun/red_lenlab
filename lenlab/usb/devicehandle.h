#ifndef USB_DEVICEHANDLE_H
#define USB_DEVICEHANDLE_H

#include "libusb.h"

namespace usb {

class DeviceHandle
{
public:
    libusb_device_handle *m_dev_handle;

    DeviceHandle(libusb_device *dev);
    DeviceHandle(const DeviceHandle&) = delete;

    ~DeviceHandle();

    DeviceHandle& operator=(const DeviceHandle&) = delete;
};

} // namespace usb

#endif // USB_DEVICEHANDLE_H
