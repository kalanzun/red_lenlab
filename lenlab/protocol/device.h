#ifndef PROTOCOL_DEVICE_H
#define PROTOCOL_DEVICE_H

#include <memory>

#include <QObject>

#include "usb/transfer.h"
#include "eventloop.h"

namespace protocol {

class Device : public QObject
{
    Q_OBJECT

    std::shared_ptr< USBThread > usb_thread;
    std::shared_ptr< usb::Interface > interface;
    std::shared_ptr< EventLoop > event_loop;

    std::unique_ptr< usb::Transfer > sender;
    std::unique_ptr< usb::Transfer > receiver0;
    std::unique_ptr< usb::Transfer > receiver1;

public:
    Device(std::shared_ptr< usb::DeviceHandle > device_handle, QObject *parent = nullptr);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    void send(std::shared_ptr< usb::Packet > packet);

signals:
    void reply(std::shared_ptr< usb::Packet > packet);
    void error();

private:
    static void rxCompleteCallback(usb::Transfer* transfer, void* object);
    static void errorCallback(usb::Transfer* transfer, void* object);
};

} // namespace protocol

#endif // PROTOCOL_DEVICE_H
