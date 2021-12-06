#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include <memory>

#include "devicehandle.h"

namespace usb {

class Interface
{
public:
    std::shared_ptr< DeviceHandle > device_handle;

    explicit Interface(std::shared_ptr< DeviceHandle >& device_handle);
    ~Interface();

    Interface(const Interface&) = delete;
    Interface& operator=(const Interface&) = delete;
};

} // namespace usb

#endif // USB_INTERFACE_H
