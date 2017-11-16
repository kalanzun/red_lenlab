#ifndef DEVHANDLE_H
#define DEVHANDLE_H

#include "libusb.h"

namespace usb {
namespace resource {

    class DevHandle
    {
        libusb_device_handle *dev_handle;

    public:
        DevHandle(libusb_device *dev);
        ~DevHandle();

        libusb_device_handle *get();

    };

}
}

#endif // DEVHANDLE_H
