#include "eventloop.h"
#include <QDebug>

using namespace usb::resource;

EventLoop::EventLoop(Thread *thread) : thread(thread)
{
    qDebug("start");
    thread->start();
}

EventLoop::~EventLoop()
{
    qDebug("request interruption");
    thread->requestInterruption();
}
