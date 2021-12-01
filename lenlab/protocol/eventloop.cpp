#include "eventloop.h"

#include <QDebug>

namespace protocol {

EventLoop::EventLoop(std::shared_ptr< Thread > thread, QObject *parent)
    : QObject{parent}
    , mThread{std::move(thread)}
{
    qDebug() << "EventLoop";

    mThread->start();
}

EventLoop::~EventLoop()
{
    qDebug() << "~EventLoop";

    mThread->requestInterruption();
}

} // namespace protocol
