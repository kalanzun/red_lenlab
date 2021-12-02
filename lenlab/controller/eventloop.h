#ifndef CONTROLLER_EVENTLOOP_H
#define CONTROLLER_EVENTLOOP_H

#include <memory>

#include <QObject>

#include "usbthread.h"

namespace controller {

class EventLoop : public QObject
{
    Q_OBJECT

    std::shared_ptr< USBThread > usb_thread;

public:
    explicit EventLoop(std::shared_ptr< USBThread > usb_thread, QObject *parent = nullptr);
    ~EventLoop();

signals:

};

} // namespace controller

#endif // CONTROLLER_EVENTLOOP_H
