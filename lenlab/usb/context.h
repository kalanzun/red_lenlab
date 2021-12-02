#ifndef USB_CONTEXT_H
#define USB_CONTEXT_H

#include "libusb.h"

namespace usb {

class Context
{
public:
    struct libusb_context* ctx;

    Context();
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
};

} // namespace usb

#endif // USB_CONTEXT_H
