#ifndef PROTOCOL_DEVICE_H
#define PROTOCOL_DEVICE_H

#include <memory>

#include <QObject>

namespace usb {

class DeviceHandle;
class Interface;
class Packet;
class Transfer;

}

namespace protocol {

class EventLoop;
class USBThread;

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
    explicit Device(std::shared_ptr< usb::DeviceHandle >& device_handle, QObject *parent = nullptr);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    void send(std::shared_ptr< usb::Packet >& packet);

signals:
    void reply(std::shared_ptr< usb::Packet >& packet);
    void error();

private:
    static void rxCompleteCallback(usb::Transfer* transfer, void* object);
    static void errorCallback(usb::Transfer* transfer, void* object);
};

} // namespace protocol

#endif // PROTOCOL_DEVICE_H
