#ifndef USB_CONTEXT_H
#define USB_CONTEXT_H

#include "libusb.h"

namespace usb {

class Context
{
    struct libusb_context* ctx;

public:
    Context();
    Context(const Context&) = delete;

    ~Context();

    Context& operator=(const Context&) = delete;
};

} // namespace usb

#endif // USB_CONTEXT_H
