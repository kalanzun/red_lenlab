#include "device.h"

#include <QDebug>

namespace controller {

Device::Device(std::shared_ptr< usb::DeviceHandle > device_handle, QObject *parent)
    : QObject{parent}
    , usb_thread{std::make_shared< USBThread >()}
    , interface{std::make_shared< usb::Interface >(std::move(device_handle))}
    , event_loop{std::make_shared< EventLoop >(usb_thread)}
    , sender{std::make_unique< usb::Transfer >(interface, 0x01)}
    , receiver0{std::make_unique< usb::Transfer >(interface, 0x81)}
    , receiver1{std::make_unique< usb::Transfer >(interface, 0x81)}
{
    qDebug() << "Device";

    sender->error_callback.set(errorCallback, this);

    receiver0->complete_callback.set(rxCompleteCallback, this);
    receiver0->error_callback.set(errorCallback, this);

    receiver1->complete_callback.set(rxCompleteCallback, this);
    receiver1->error_callback.set(errorCallback, this);

    receiver0->submit(std::make_shared< usb::Packet >());
    receiver1->submit(std::make_shared< usb::Packet >());
}

Device::~Device()
{
    qDebug() << "~Device";

    sender->cancel();
    receiver0->cancel();
    receiver1->cancel();
}

void Device::send(std::shared_ptr< usb::Packet > packet)
{
    sender->submit(std::move(packet));
}

void Device::rxCompleteCallback(usb::Transfer* transfer, void* object)
{
    // callback runs in the thread
    qDebug() << "callbackRxComplete" << transfer->packet;

    // signal back to main thread
    auto device = static_cast< Device* >(object);
    emit device->reply(transfer->packet);

    // create new transfer->packet
    transfer->submit(std::make_shared< usb::Packet >());
}

void Device::errorCallback(usb::Transfer* transfer, void* object)
{
    // callback runs in the thread
    qDebug() << "callbackError" << transfer->getErrorMessage();

    // signal back to main thread
    auto device = static_cast< Device* >(object);
    emit device->error();
}

} // namespace controller
