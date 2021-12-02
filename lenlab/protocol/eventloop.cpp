#include "eventloop.h"

namespace protocol {

EventLoop::EventLoop(std::shared_ptr< USBThread > usb_thread, QObject *parent)
    : QObject{parent}
    , usb_thread{std::move(usb_thread)}
{
    this->usb_thread->start();
}

EventLoop::~EventLoop()
{
    usb_thread->requestInterruption();
}

} // namespace protocol