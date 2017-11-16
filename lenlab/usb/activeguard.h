#ifndef ACTIVEGUARD_H
#define ACTIVEGUARD_H

#include "active.h"

namespace usb {
namespace resource {

    class ActiveGuard
    {
        Active *active;

    public:
        ActiveGuard(Active *active);
        ~ActiveGuard();

    };

}
}

#endif // ACTIVEGUARD_H
