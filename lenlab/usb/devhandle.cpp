#include "devhandle.h"
#include "exception.h"
#include <QDebug>

using namespace usb::resource;

DevHandle::DevHandle(libusb_device *dev)
{
    qDebug("open");
    auto err = libusb_open(dev, &dev_handle);
    if (err) throw Exception(libusb_strerror((libusb_error) err));
}

DevHandle::~DevHandle()
{
    qDebug("close");
    libusb_close(dev_handle);
}

libusb_device_handle *
DevHandle::get()
{
    return dev_handle;
}
