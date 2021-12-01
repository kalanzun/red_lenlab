#include "device.h"

#include <QDebug>

namespace protocol {

Device::Device(std::shared_ptr< usb::DeviceHandle > device_handle, QObject *parent)
    : QObject{parent}
    , mThread{std::make_shared< Thread >()}
    , interface{std::make_shared< usb::Interface >(std::move(device_handle))}
    , mEventLoop{std::make_shared< EventLoop >(mThread)}
    , sender{interface, 0x01}
    , receiver0{interface, 0x81}
    , receiver1{interface, 0x81}
{
    qDebug() << "Device";

    sender.error_callback.set(this, callbackError);
    sender.reset_callback.set(this, callbackReset);

    receiver0.complete_callback.set(this, callbackRxComplete);
    receiver0.error_callback.set(this, callbackError);
    receiver0.reset_callback.set(this, callbackReset);

    receiver1.complete_callback.set(this, callbackRxComplete);
    receiver1.error_callback.set(this, callbackError);
    receiver1.reset_callback.set(this, callbackReset);

    receiver0.submit(std::make_shared< usb::Packet >());
    receiver1.submit(std::make_shared< usb::Packet >());
}

Device::~Device()
{
    qDebug() << "~Device";

    sender.cancel();
    receiver0.cancel();
    receiver1.cancel();
}

void Device::send(std::shared_ptr< usb::Packet > packet)
{
    sender.submit(std::move(packet));
}

void Device::callbackRxComplete(usb::Transfer* transfer, void* object)
{
    auto device = static_cast< Device* >(object);
    qDebug() << "callbackRxComplete" << transfer->getPacket();

    transfer->submit(std::make_shared< usb::Packet >());
}

void Device::callbackError(usb::Transfer* transfer, void* object)
{
    auto device = static_cast< Device* >(object);
    qDebug() << "callbackError" << transfer->getMessage();
}

void Device::callbackReset(usb::Transfer* transfer, void* object)
{
    auto device = static_cast< Device* >(object);
    qDebug() << "callbackReset";

    emit device->reset();
}

} // namespace protocol
