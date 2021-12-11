#ifndef PROTOCOL_USBDEVICE_H
#define PROTOCOL_USBDEVICE_H

#include "device.h"

namespace usb {
class DeviceHandle;
class Interface;
class Transfer;
}

namespace protocol {

class EventLoop;
class USBThread;

class USBDevice : public Device
{
    Q_OBJECT

    std::shared_ptr< USBThread > usb_thread;
    std::shared_ptr< usb::Interface > interface;
    std::shared_ptr< EventLoop > event_loop;

    std::unique_ptr< usb::Transfer > sender;
    std::unique_ptr< usb::Transfer > receiver0;
    std::unique_ptr< usb::Transfer > receiver1;

public:
    explicit USBDevice(std::shared_ptr< usb::DeviceHandle >& device_handle, QObject *parent = nullptr);
    ~USBDevice();

public slots:
    void send(std::shared_ptr< Message >& message);

private:
    static void rxCompleteCallback(usb::Transfer* transfer, void* object);
    static void errorCallback(usb::Transfer* transfer, void* object);
};

} // namespace protocol

#endif // PROTOCOL_USBDEVICE_H
