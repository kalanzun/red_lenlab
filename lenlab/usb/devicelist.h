#ifndef USB_DEVICELIST_H
#define USB_DEVICELIST_H

#include "libusb.h"

namespace usb {

class DeviceListIterator
{
    libusb_device** list;
    ssize_t i;

public:
    DeviceListIterator(libusb_device** list, ssize_t i);

    DeviceListIterator(const DeviceListIterator&) = delete;
    DeviceListIterator& operator=(const DeviceListIterator&) = delete;

    bool operator!=(const DeviceListIterator& other);
    DeviceListIterator& operator++();
    libusb_device* operator*();
};

class DeviceList
{
    libusb_device** list;
    ssize_t length;

public:
    DeviceList();
    ~DeviceList();

    DeviceList(const DeviceList&) = delete;
    DeviceList& operator=(const DeviceList&) = delete;

    DeviceListIterator begin() const;
    DeviceListIterator end() const;
};

} // namespace usb

#endif // USB_DEVICELIST_H
