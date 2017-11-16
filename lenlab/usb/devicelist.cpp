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
    count = libusb_get_device_list(NULL, &list);
    if (!list) throw Exception();
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
