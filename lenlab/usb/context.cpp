#include "context.h"
#include "exception.h"

using namespace usb::resource;

Context::Context()
{
    int err = libusb_init(&ctx);
    if (err) throw Exception();
}

Context::~Context()
{
    libusb_exit(ctx);
}
