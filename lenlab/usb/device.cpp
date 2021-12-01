#include "device.h"

#include <QDebug>

#include "devicelist.h"
#include "transfer.h"

namespace usb {

Device::Device(libusb_device* dev)
    : m_device_handle(dev)
    , m_interface(m_device_handle)
{
    receive();
    receive();
}

void Device::receiveCallback(void* object, std::shared_ptr< Packet >& packet)
{
    auto device = static_cast< Device* >(object);
    qDebug() << "Device::receiveCallback";
    device->receive();
    if (device->m_receive_callback) device->m_receive_callback(device->m_object, packet);
}

void Device::receive()
{
    auto packet = std::make_shared< Packet >();
    packet->length = 1024;
    auto rx = new Transfer(packet, m_device_handle, 0x81);
    rx->m_object = this;
    rx->m_receive_callback = &receiveCallback;
    rx->submit();
}

std::shared_ptr< Device > Device::query(uint16_t vid, uint16_t pid)
{
    struct libusb_device_descriptor desc;
    DeviceList list; // may throw

    for(auto dev: list) {
        libusb_get_device_descriptor(dev, &desc);

        if(desc.idVendor == vid && desc.idProduct == pid) {
            return std::make_shared< Device >(dev); // may throw
        }
    }

    return nullptr;
}

void Device::transmit(std::shared_ptr< Packet >& packet)
{
    auto tx = new Transfer(packet, m_device_handle, 0x01); // it deletes itself
    tx->submit();
}

} // namespace usb
