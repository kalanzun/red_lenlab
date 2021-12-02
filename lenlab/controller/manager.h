#ifndef CONTROLLER_MANAGER_H
#define CONTROLLER_MANAGER_H

#include <memory>

#include <QObject>
#include <QTimer>

#include "usb/context.h"
#include "usb/packet.h"

#include "device.h"

namespace controller {

class Manager : public QObject
{
    Q_OBJECT

    static int const poll_time = 300;
    static int const retry_time = 1000;

    std::unique_ptr< usb::Context > context;

    QTimer *poll_timer;

    std::shared_ptr< Device > device = nullptr;

public:
    explicit Manager(QObject *parent = nullptr);

    void command(std::shared_ptr< usb::Packet > packet);

signals:
    void setup(std::shared_ptr< usb::Packet > packet);
    void reply(std::shared_ptr< usb::Packet > packet);
    void teardown();

public slots:
    void lookForBoard();
    void handleReply(std::shared_ptr< usb::Packet > packet);

private slots:
    void poll();
    void clearDevice();
};

} // namespace controller

#endif // CONTROLLER_MANAGER_H
