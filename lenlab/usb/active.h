#ifndef ACTIVE_H
#define ACTIVE_H

#include <mutex>
#include <condition_variable>

namespace usb {

    class Active
    {
        std::mutex m;
        std::condition_variable cv;
        bool active = false;

    public:
        Active();

        void lock();
        void unlock();
        bool isActive();
        void wait();
    };

}

#endif // ACTIVE_H
