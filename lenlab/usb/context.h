#ifndef CONTEXT_H
#define CONTEXT_H

#include "libusb.h"

namespace usb {
namespace resource {

    class Context
    {
        struct libusb_context *ctx;

    public:
        Context();
        ~Context();
    };

}
}

#endif // CONTEXT_H
