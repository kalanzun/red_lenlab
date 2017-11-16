#include "devhandle.h"
#include "exception.h"
#include <QDebug>

using namespace usb::resource;

DevHandle::DevHandle(libusb_device *dev)
{
    qDebug("open");
    int err = libusb_open(dev, &dev_handle);
    if (err) throw Exception();
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
