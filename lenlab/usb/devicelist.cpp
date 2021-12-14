#include "devicelist.h"

#include "usbexception.h"

namespace usb {

DeviceListIterator::DeviceListIterator(libusb_device** list, ssize_t i)
    : list{list}
    , i{i}
{

}

bool DeviceListIterator::operator!=(const DeviceListIterator& other)
{
    return i != other.i;
}

DeviceListIterator& DeviceListIterator::operator++() {
    ++i;
    return *this;
}

libusb_device* DeviceListIterator::operator*()
{
    return list[i];
}

DeviceList::DeviceList()
{
    length = libusb_get_device_list(nullptr, &list);
    if (length < 0) throw USBException(length);
}

DeviceList::~DeviceList()
{
    libusb_free_device_list(list, true);
}

DeviceListIterator DeviceList::begin() const
{
    return DeviceListIterator{list, 0};
}

DeviceListIterator DeviceList::end() const
{
    return DeviceListIterator{list, length};
}

} // namespace usb
