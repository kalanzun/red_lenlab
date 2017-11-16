#ifndef USBDEVICELIST_H
#define USBDEVICELIST_H

#include "libusb.h"

namespace usb {
namespace resource {

    class DeviceListIterator
    {
        libusb_device **list = nullptr;
        size_t i;

    public:
        DeviceListIterator(libusb_device **list, size_t i);

        bool operator !=(DeviceListIterator const &other);
        DeviceListIterator& operator ++();
        libusb_device* operator *();
    };

    class DeviceList
    {
        libusb_device **list = nullptr;
        size_t count;

    public:
        DeviceList();
        ~DeviceList();

        DeviceListIterator begin();
        DeviceListIterator end();
    };

}
}

#endif // USBDEVICELIST_H
