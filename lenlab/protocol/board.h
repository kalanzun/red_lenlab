#ifndef PROTOCOL_BOARD_H
#define PROTOCOL_BOARD_H

#include <QObject>
#include <QTimer>

#include "usb/context.h"
#include "usb/device.h"
#include "usb/packet.h"

namespace protocol {

class Board : public QObject
{
    Q_OBJECT

    static int const mPollTime = 300;
    static int const mRetryTime = 3000;

    usb::Context m_context;

    QTimer mPollTimer;

public:
    explicit Board(QObject *parent = nullptr);

signals:
    void transmit(std::shared_ptr< usb::Packet >& packet);

public slots:
    void lookForBoard();

private slots:
    void poll();
    void receive(std::shared_ptr< usb::Packet >& packet);

};

} // namespace protocol

#endif // PROTOCOL_BOARD_H
