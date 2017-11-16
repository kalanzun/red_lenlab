#ifndef INTERFACE_H
#define INTERFACE_H

#include "libusb.h"

namespace usb {
namespace resource {

    class Interface
    {
        libusb_device_handle *dev_handle;

    public:
        Interface(libusb_device_handle *dev_handle);
        ~Interface();
    };

}
}

#endif // INTERFACE_H
