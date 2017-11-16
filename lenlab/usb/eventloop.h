#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "thread.h"

namespace usb {
namespace resource {

    class EventLoop
    {
        Thread *thread;

    public:
        EventLoop(Thread *thread);
        ~EventLoop();
    };

}
}

#endif // EVENTLOOP_H
