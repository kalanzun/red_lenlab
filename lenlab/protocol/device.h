#ifndef PROTOCOL_DEVICE_H
#define PROTOCOL_DEVICE_H

#include <memory>

#include <QObject>

#include "usb/devicehandle.h"
#include "usb/interface.h"
#include "usb/packet.h"
#include "usb/transfer.h"

#include "eventloop.h"
#include "thread.h"

namespace protocol {

class Device : public QObject
{
    Q_OBJECT

    std::shared_ptr< Thread > mThread;
    std::shared_ptr< usb::Interface > interface;
    std::shared_ptr< EventLoop > mEventLoop;

    usb::Transfer sender;
    usb::Transfer receiver0;
    usb::Transfer receiver1;

public:
    Device(std::shared_ptr< usb::DeviceHandle > device_handle, QObject *parent = nullptr);
    Device(const Device&) = delete;

    ~Device();

    Device& operator=(const Device&) = delete;

    void send(std::shared_ptr< usb::Packet > packet);

signals:
    void reset();

private:
    static void callbackRxComplete(usb::Transfer* transfer, void* object);
    static void callbackError(usb::Transfer* transfer, void* object);
    static void callbackReset(usb::Transfer* transfer, void* object);
};

} // namespace protocol

#endif // PROTOCOL_DEVICE_H
