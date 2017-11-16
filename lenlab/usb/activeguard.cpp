#include "activeguard.h"
#include <QDebug>
#include <memory>

using namespace usb::resource;

ActiveGuard::ActiveGuard(Active *active) :
    active(active)
{
    active->lock();
}

ActiveGuard::~ActiveGuard()
{
    active->unlock();
}
