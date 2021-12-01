#ifndef PROTOCOL_EVENTLOOP_H
#define PROTOCOL_EVENTLOOP_H

#include <memory>

#include <QObject>

#include "thread.h"

namespace protocol {

class EventLoop : public QObject
{
    Q_OBJECT

    std::shared_ptr< Thread > mThread;

public:
    explicit EventLoop(std::shared_ptr< Thread > thread, QObject *parent = nullptr);
    ~EventLoop();

signals:

};

} // namespace protocol

#endif // PROTOCOL_EVENTLOOP_H
