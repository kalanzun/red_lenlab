#include "interface.h"

#include <QDebug>

#include "libusb.h"

#include "usbexception.h"

namespace usb {

Interface::Interface(std::shared_ptr< DeviceHandle > device_handle)
    : device_handle{std::move(device_handle)}
{
    qDebug() << "Interface";

    auto error = libusb_claim_interface(this->device_handle->dev_handle, 0);
    if (error) throw USBException(error);
}

Interface::~Interface()
{
    qDebug() << "~Interface";

    libusb_release_interface(device_handle->dev_handle, 0);
}

} // namespace usb
