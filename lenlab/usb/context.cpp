#include "context.h"

#include "usbexception.h"

namespace usb {

Context::Context()
{
    auto error = libusb_init(&m_ctx);
    if (error) throw USBException(error);
}

Context::~Context()
{
    libusb_exit(m_ctx);
}

} // namespace usb
