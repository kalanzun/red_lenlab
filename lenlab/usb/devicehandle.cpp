#include "devicehandle.h"

#include <QDebug>

#include "devicelist.h"
#include "usbexception.h"

namespace usb {

DeviceHandle::DeviceHandle(libusb_device* dev)
{
    qDebug() << "DeviceHandle";

    auto error = libusb_open(dev, &dev_handle);
    if (error) throw USBException(error);
}

DeviceHandle::~DeviceHandle()
{
    qDebug() << "~DeviceHandle";

    libusb_close(dev_handle);
}

std::shared_ptr< DeviceHandle > DeviceHandle::query(uint16_t vid, uint16_t pid)
{
    struct libusb_device_descriptor desc;
    DeviceList list; // may throw

    for(auto dev: list) {
        libusb_get_device_descriptor(dev, &desc);

        if(desc.idVendor == vid && desc.idProduct == pid) {
            return std::make_shared< DeviceHandle >(dev); // may throw
        }
    }

    return nullptr;
}

} // namespace usb
