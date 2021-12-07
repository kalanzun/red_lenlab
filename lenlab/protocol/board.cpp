#include "board.h"

#include "lenlab_protocol.h"
#include "usb/devicehandle.h"
#include "usb/packet.h"
#include "usb/usbexception.h"
#include "device.h"
#include "querythread.h"

namespace protocol {

Board::Board(QObject *parent)
    : QObject{parent}
    , query_thread{std::make_unique< QueryThread >(LENLAB_VID, LENLAB_PID)}
{
    connect(query_thread.get(), &QueryThread::DeviceHandleCreated,
            this, &Board::setupDevice);
}

void Board::command(std::shared_ptr< usb::Packet >& packet)
{
    assert(device);

    if (device) device->send(packet);
}

void Board::lookForDevice()
{
    assert(!query_thread->isRunning());

    query_thread->start();
}

void Board::setupDevice(std::shared_ptr< usb::DeviceHandle >& device_handle)
{
    assert(!device);

    device = new Device(device_handle, this);
    connect(device.get(), &Device::reply, this, &Board::handleReply);
    connect(device.get(), &Device::error, this, &Board::handleError);
    connect(device.get(), &Device::destroyed, this, &Board::lookForDevice);

    auto packet = std::make_shared< usb::Packet >();
    packet->buffer[0] = setUp;
    packet->buffer[1] = nullType;
    packet->buffer[2] = 0;
    packet->buffer[3] = 0;
    packet->length = 4;
    device->send(packet);
}

void Board::handleReply(std::shared_ptr< usb::Packet >& packet)
{
    if (packet->buffer[0] == setUp) {
        emit setup(packet);
    }
    else {
        emit reply(packet);
    }
}

void Board::handleError()
{
    assert(device);

    device->deleteLater();

    emit error();
}

} // namespace protocol
