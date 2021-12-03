#ifndef USB_DEVICEHANDLE_H
#define USB_DEVICEHANDLE_H

#include <memory>

#include "libusb.h"

namespace usb {

class DeviceHandle
{
public:
    libusb_device_handle* dev_handle;

    explicit DeviceHandle(libusb_device* dev);
    ~DeviceHandle();

    DeviceHandle(const DeviceHandle&) = delete;
    DeviceHandle& operator=(const DeviceHandle&) = delete;

    static std::shared_ptr< DeviceHandle > query(uint16_t vid, uint16_t pid);
};

} // namespace usb

#endif // USB_DEVICEHANDLE_H
