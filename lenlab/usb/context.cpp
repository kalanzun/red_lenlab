#include "context.h"
#include "exception.h"

using namespace usb::resource;

Context::Context()
{
    auto err = libusb_init(&ctx);
    if (err) throw Exception(libusb_strerror((libusb_error) err));
}

Context::~Context()
{
    libusb_exit(ctx);
}
