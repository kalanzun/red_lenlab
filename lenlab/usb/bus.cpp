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

#include "bus.h"
#include "devicelist.h"
#include "usberror.h"

using namespace usb;

Bus::Bus() : context()
{

}

pDevice
Bus::query(uint16_t vid, uint16_t pid)
{
    struct libusb_device_descriptor desc;
    resource::DeviceList device_list; // may throw

    for(auto dev: device_list) {
        libusb_get_device_descriptor(dev, &desc);

        // Is it our device?
        if(desc.idVendor == vid && desc.idProduct == pid) {
            return pDevice(new Device(dev)); // may throw
        }
    }

    throw NotFound();
}
