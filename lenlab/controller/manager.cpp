#include "manager.h"

#include "lenlab_protocol.h"

#include "usb/usbexception.h"

namespace controller {

Manager::Manager(QObject *parent)
    : QObject{parent}
    , context{std::make_unique< usb::Context >()}
    , poll_timer{new QTimer(this)}
{
    connect(poll_timer, &QTimer::timeout,
            this, &Manager::poll);

    poll_timer->setSingleShot(true);
}

void Manager::command(std::shared_ptr< usb::Packet > packet)
{
    if (device) device->send(std::move(packet));
}

void Manager::lookForBoard()
{
    poll_timer->start(poll_time);
}

void Manager::handleReply(std::shared_ptr< usb::Packet > packet)
{
    qDebug() << "handleReply" << packet;

    if (packet->buffer[0] == setUp) {
        emit setup(std::move(packet));
    }
    else {
        emit reply(std::move(packet));
    }
}

void Manager::poll()
{
    try {
        auto device_handle = usb::DeviceHandle::query(LENLAB_VID, LENLAB_PID);
        if (device_handle) {
            qDebug() << "board connected";

            device = std::make_shared< Device >(std::move(device_handle));
            connect(device.get(), &Device::reply, this, &Manager::handleReply);
            connect(device.get(), &Device::error, this, &Manager::clearDevice);

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
        qDebug() << exception.message;
        poll_timer->start(retry_time);
    }
}

void Manager::clearDevice()
{
    emit teardown();
    device = nullptr;
    poll_timer->start(retry_time);
}

} // namespace controller
