#ifndef NOTETOSELF_H
#define NOTETOSELF_H

#include "active.h"
#include "activeguard.h"
#include "message.h"
#include "transfer.h"

namespace usb {

    class NoteToSelf
    {
    public:
        NoteToSelf(Transfer *transfer, Active *active, pMessage message);

        Transfer *transfer;
        resource::ActiveGuard active_guard;
        pMessage message;
    };

}

#endif // NOTETOSELF_H
