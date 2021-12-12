#ifndef PROTOCOL_BOARD_H
#define PROTOCOL_BOARD_H

#include <memory>

#include <QObject>

namespace usb{
class DeviceHandle;
}

namespace protocol {

class Device;
class Message;
class QueryThread;

class Board : public QObject
{
    Q_OBJECT

    std::unique_ptr< QueryThread > query_thread;
    std::shared_ptr< Device > device = nullptr;

public:
    explicit Board(QObject *parent = nullptr);

    void command(std::shared_ptr< Message >& message);

signals:
    void setup(std::shared_ptr< Message >& message);
    void reply(std::shared_ptr< Message >& message);
    void error();

public slots:
    void lookForDevice(bool create_virtual_device = false);

private slots:
    void handleDeviceHandleCreated(std::shared_ptr< usb::DeviceHandle > device_handle);
    void handleQueryThreadStatistics(int count, int interval, int runtime);
    void handleReply(std::shared_ptr< Message >& message);
    void handleError();

private:
    void setupDevice();
};

} // namespace protocol

#endif // PROTOCOL_BOARD_H
