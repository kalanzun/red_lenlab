#ifndef PROTOCOL_QUERYTHREAD_H
#define PROTOCOL_QUERYTHREAD_H

#include <memory>

#include <QThread>

namespace usb {

class DeviceHandle;

} // namespace usb

namespace protocol {

class QueryThread : public QThread
{
    Q_OBJECT

    uint16_t vid;
    uint16_t pid;

public:
    explicit QueryThread(uint16_t vid, uint16_t pid, QObject *parent = nullptr);
    ~QueryThread();

    void run() override;

signals:
    void DeviceHandleCreated(std::shared_ptr< usb::DeviceHandle >& device_handle);
};

} // namespace protocol

#endif // PROTOCOL_QUERYTHREAD_H
