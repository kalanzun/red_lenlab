#include "virtualdevice.h"

#include <QDebug>

#include "usb/packet.h"
#include "message.h"

namespace protocol {

VirtualDevice::VirtualDevice(QObject *parent)
    : Device{parent}
{
    connect(this, &VirtualDevice::command,
            this, &VirtualDevice::handleCommand,
            Qt::QueuedConnection);
}

void VirtualDevice::send(const std::shared_ptr< Message >& message)
{
    emit command(message);
}

void VirtualDevice::handleCommand(const std::shared_ptr< Message >& cmd)
{
    auto rpl = std::make_shared< Message >();
    rpl->head->type = nullType;
    rpl->head->reference = cmd->head->reference;
    rpl->packet->length = 4;

    switch (cmd->head->command) {
    case setUp:
        qDebug() << "setUp";
        rpl->head->reply = Setup;
        emit reply(rpl);
        break;
    default:
        break;
    }
}


} // namespace protocol
