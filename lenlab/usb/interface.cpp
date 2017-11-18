#include "interface.h"
#include "exception.h"
#include <QDebug>

using namespace usb::resource;

Interface::Interface(libusb_device_handle *dev_handle) : dev_handle(dev_handle)
{
    qDebug("claim");
    auto err = libusb_claim_interface(dev_handle, 0);
    if (err) throw Exception(libusb_strerror((libusb_error) err));
}

Interface::~Interface()
{
    qDebug("release");
    libusb_release_interface(dev_handle, 0);
}
