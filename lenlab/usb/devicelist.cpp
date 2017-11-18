#include "devicelist.h"
#include "exception.h"

using namespace usb::resource;

DeviceListIterator::DeviceListIterator(libusb_device **list, size_t i) : list(list), i(i) {}

bool
DeviceListIterator::operator !=(DeviceListIterator const &other)
{
    return i != other.i;
}

DeviceListIterator&
DeviceListIterator::operator ++() {
    ++i;
    return *this;
}

libusb_device*
DeviceListIterator::operator *()
{
    return list[i];
}

DeviceList::DeviceList()
{
    auto err = libusb_get_device_list(NULL, &list);
    if (err < 0) throw Exception(libusb_strerror((libusb_error) err));
    else count = err;
}

DeviceList::~DeviceList()
{
    libusb_free_device_list(list, true);
}

DeviceListIterator
DeviceList::begin()
{
    return DeviceListIterator(list, 0);
}

DeviceListIterator
DeviceList::end()
{
    return DeviceListIterator(list, count);
}
