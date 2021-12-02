#include "board.h"

#include "lenlab_protocol.h"
#include "usb/usbexception.h"

namespace protocol {

Board::Board(QObject *parent)
    : QObject{parent}
    , poll_timer{new QTimer(this)}
{
    connect(poll_timer, &QTimer::timeout,
            this, &Board::poll);

    poll_timer->setSingleShot(true);
}

void Board::command(std::shared_ptr< usb::Packet > packet)
{
    if (device) device->send(std::move(packet));
}

void Board::lookForDevice()
{
    poll_timer->start(poll_time);
}

void Board::handleReply(std::shared_ptr< usb::Packet > packet)
{
    if (packet->buffer[0] == setUp) {
        emit setup(std::move(packet));
    }
    else {
        emit reply(std::move(packet));
    }
}

void Board::poll()
{
    try {
        auto device_handle = usb::DeviceHandle::query(LENLAB_VID, LENLAB_PID);
        if (device_handle) {
            device = std::make_shared< Device >(std::move(device_handle));
            connect(device.get(), &Device::reply, this, &Board::handleReply);
            connect(device.get(), &Device::error, this, &Board::clearDevice);

            auto packet = std::make_shared< usb::Packet >();
            packet->buffer[0] = setUp;
            packet->buffer[1] = nullType;
            packet->buffer[2] = 0;
            packet->buffer[3] = 0;
            packet->length = 4;
            device->send(std::move(packet));
        }
        else {
            poll_timer->start(poll_time);
        }
    } catch (usb::USBException const &exception) {
        poll_timer->start(retry_time);
    }
}

void Board::clearDevice()
{
    emit teardown();
    device = nullptr;
    poll_timer->start(retry_time);
}

} // namespace protocol
