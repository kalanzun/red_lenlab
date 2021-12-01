#include "context.h"

#include <QDebug>

#include "usbexception.h"

namespace usb {

Context::Context()
{
    qDebug() << "Context";

    auto error = libusb_init(&ctx);
    if (error) throw USBException(error);
}

Context::~Context()
{
    qDebug() << "~Context";

    libusb_exit(ctx);
}

} // namespace usb
