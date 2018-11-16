/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "devicelist.h"
#include "exception.h"

using namespace usb::resource;

DeviceListIterator::DeviceListIterator(libusb_device **list, int i) : list(list), i(i) {}

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
    auto err = libusb_get_device_list(nullptr, &list);
    if (err < 0) throw Exception(libusb_strerror(static_cast<libusb_error>(err)));
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
