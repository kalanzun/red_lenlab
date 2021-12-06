#ifndef PROTOCOL_EVENTLOOP_H
#define PROTOCOL_EVENTLOOP_H

#include <memory>

#include <QObject>

#include "usbthread.h"

namespace protocol {

class EventLoop : public QObject
{
    Q_OBJECT

    std::shared_ptr< USBThread > usb_thread;

public:
    explicit EventLoop(std::shared_ptr< USBThread >& usb_thread, QObject *parent = nullptr);
    ~EventLoop();

signals:

};

} // namespace protocol

#endif // PROTOCOL_EVENTLOOP_H
