#include "active.h"
#include <QDebug>

using namespace usb;

Active::Active()
{

}

void
Active::lock()
{
    std::lock_guard<std::mutex> lock(m);
    active = true;
    //qDebug("lock");
}

void
Active::unlock()
{
    {
        std::lock_guard<std::mutex> lock(m);
        active = false;
        //qDebug("unlock");
    }
    cv.notify_one();
}

bool
Active::isActive()
{
    std::lock_guard<std::mutex> lock(m);
    return active;
}

void
Active::wait()
{
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [this]{ return !active; });
}
