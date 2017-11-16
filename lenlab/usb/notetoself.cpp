#include "notetoself.h"

using namespace usb;

NoteToSelf::NoteToSelf(Transfer *transfer, Active *active, pMessage message) :
    transfer(transfer),
    active_guard(active),
    message(message)
{

}
