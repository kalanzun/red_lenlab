#ifndef PROTOCOL_BOARD_H
#define PROTOCOL_BOARD_H

#include <memory>

#include <QObject>
#include <QTimer>

#include "device.h"

namespace protocol {

class Board : public QObject
{
    Q_OBJECT

    static int const poll_time = 300;
    static int const retry_time = 1000;

    QTimer *poll_timer;
    std::shared_ptr< Device > device = nullptr;

public:
    explicit Board(QObject *parent = nullptr);

    void command(std::shared_ptr< usb::Packet > packet);

signals:
    void setup(std::shared_ptr< usb::Packet > packet);
    void reply(std::shared_ptr< usb::Packet > packet);
    void teardown();

public slots:
    void lookForDevice();
    void handleReply(std::shared_ptr< usb::Packet > packet);

private slots:
    void poll();
    void clearDevice();
};

} // namespace protocol

#endif // PROTOCOL_BOARD_H
