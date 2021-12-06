#include "devicehandle.h"

#include "devicelist.h"
#include "usbexception.h"

namespace usb {

DeviceHandle::DeviceHandle(libusb_device* dev)
{
    auto error = libusb_open(dev, &dev_handle);
    if (error) throw USBException(error);
}

DeviceHandle::~DeviceHandle()
{
    libusb_close(dev_handle);
}

} // namespace usb
