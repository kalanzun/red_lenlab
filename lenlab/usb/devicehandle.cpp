#include "devicehandle.h"

#include "usbexception.h"

namespace usb {

DeviceHandle::DeviceHandle(libusb_device* dev)
{
    auto error = libusb_open(dev, &m_dev_handle);
    if (error) throw USBException(error);
}

DeviceHandle::~DeviceHandle()
{
    libusb_close(m_dev_handle);
}

} // namespace usb
