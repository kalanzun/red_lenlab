#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include "libusb.h"

#include "devicehandle.h"

namespace usb {

class Interface
{
    libusb_device_handle *m_dev_handle;

public:
    Interface(DeviceHandle &device_handle);
    Interface(const Interface&) = delete;

    ~Interface();

    Interface& operator=(const Interface&) = delete;
};

} // namespace usb

#endif // USB_INTERFACE_H
