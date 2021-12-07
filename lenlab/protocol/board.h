#ifndef PROTOCOL_BOARD_H
#define PROTOCOL_BOARD_H

#include <memory>

#include <QObject>
#include <QPointer>

namespace usb {

class DeviceHandle;
class Packet;

}

namespace protocol {

class Device;
class QueryThread;

class Board : public QObject
{
    Q_OBJECT

    std::unique_ptr< QueryThread > query_thread;
    QPointer< Device > device;

public:
    explicit Board(QObject *parent = nullptr);

    void command(std::shared_ptr< usb::Packet >& packet);

signals:
    void setup(std::shared_ptr< usb::Packet >& packet);
    void reply(std::shared_ptr< usb::Packet >& packet);
    void error();

public slots:
    void lookForDevice();

private slots:
    void setupDevice(std::shared_ptr< usb::DeviceHandle >& device_handle);
    void handleReply(std::shared_ptr< usb::Packet >& packet);
    void handleError();
};

} // namespace protocol

#endif // PROTOCOL_BOARD_H
