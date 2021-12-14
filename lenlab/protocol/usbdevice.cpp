#include "usbdevice.h"

#include "usb/devicehandle.h"
#include "usb/interface.h"
#include "usb/packet.h"
#include "usb/transfer.h"
#include "eventloop.h"
#include "message.h"
#include "usbthread.h"

namespace protocol {

USBDevice::USBDevice(std::shared_ptr< usb::DeviceHandle > device_handle, QObject *parent)
    : Device{parent}
    , usb_thread{std::make_shared< USBThread >()}
    , interface{std::make_shared< usb::Interface >(std::move(device_handle))}
    , event_loop{std::make_shared< EventLoop >(usb_thread)}
    , sender{std::make_unique< usb::Transfer >(interface, 0x01)}
    , receiver0{std::make_unique< usb::Transfer >(interface, 0x81)}
    , receiver1{std::make_unique< usb::Transfer >(interface, 0x81)}
{
    sender->error_callback.set(errorCallback, this);

    receiver0->complete_callback.set(rxCompleteCallback, this);
    receiver0->error_callback.set(errorCallback, this);

    receiver1->complete_callback.set(rxCompleteCallback, this);
    receiver1->error_callback.set(errorCallback, this);

    receiver0->submit();
    receiver1->submit();
}

USBDevice::~USBDevice()
{
    sender->cancel();
    receiver0->cancel();
    receiver1->cancel();
}

void USBDevice::send(const std::shared_ptr< Message >& message)
{
    sender->submit(message->packet);
}

void USBDevice::rxCompleteCallback(usb::Transfer* transfer, void* object)
{
    // callback runs in the thread
    // signal back to main thread
    auto device = static_cast< Device* >(object);
    auto message = std::make_shared< Message >(transfer->packet);
    emit device->reply(message);

    // create new transfer->packet
    transfer->submit();
}

void USBDevice::errorCallback(usb::Transfer* transfer, void* object)
{
    // callback runs in the thread
    // signal back to main thread
    auto device = static_cast< Device* >(object);
    emit device->error();
}

} // namespace protocol
