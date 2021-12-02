#include "context.h"

#include "usbexception.h"

namespace usb {

Context::Context()
{
    auto error = libusb_init(&ctx);
    if (error) throw USBException(error);
}

Context::~Context()
{
    libusb_exit(ctx);
}

} // namespace usb
