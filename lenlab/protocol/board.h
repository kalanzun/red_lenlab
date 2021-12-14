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

struct QueryThreadStatistics {
    int count;
    int interval;
    int runtime;
    int runtime_min;
    int runtime_max;
};

class Board : public QObject
{
    Q_OBJECT

    std::unique_ptr< QueryThread > query_thread;
    std::shared_ptr< Device > device = nullptr;

public:
    struct QueryThreadStatistics query_thread_statistics;

    explicit Board(QObject *parent = nullptr);

signals:
    void setup(const std::shared_ptr< Message >& message);
    void reply(const std::shared_ptr< Message >& message);
    void error();

public slots:
    void lookForDevice(bool create_virtual_device = false);
    void send(const std::shared_ptr< Message >& message);

private:
    void setupDevice();

private slots:
    void handleDeviceHandleCreated(const std::shared_ptr< usb::DeviceHandle >& device_handle);
    void handleQueryThreadStatistics(int count, int interval, int runtime);
    void handleReply(const std::shared_ptr< Message >& message);
    void handleError();
};

} // namespace protocol

#endif // PROTOCOL_BOARD_H
