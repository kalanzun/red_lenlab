#include "virtualdevice.h"

#include <cmath>

#include <QDebug>

#include "usb/packet.h"
#include "message.h"

namespace protocol {

const float VirtualDevice::pi = std::acos(-1.f);
const float VirtualDevice::f = 0.001f;


VirtualDevice::VirtualDevice(QObject *parent)
    : Device{parent}
{
    connect(this, &VirtualDevice::command,
            this, &VirtualDevice::handleCommand,
            Qt::QueuedConnection);

    connect(&logger, &QTimer::timeout,
            this, &VirtualDevice::logger_main);
}

void VirtualDevice::send(const std::shared_ptr< Message >& message)
{
    emit command(message);
}

void VirtualDevice::handleCommand(const std::shared_ptr< Message >& cmd)
{
    switch (cmd->head->command) {
    case setUp:
        set_up(cmd);
        break;
    case startLogger:
        start_logger(cmd);
        break;
    case stopLogger:
        stop_logger(cmd);
        break;
    case startOscilloscope:
        start_oscilloscope(cmd);
        break;
    default:
        break;
    }
}

void VirtualDevice::logger_main()
{
    auto rpl = Message::createReply(Log, IntArray, 0);

    rpl->addInt(count);
    for (auto i = 0; i < 4; ++i) rpl->addInt(4096 * (count % 10) / 10);

    ++count;

    emit reply(rpl);
}

void VirtualDevice::set_up(const std::shared_ptr< Message >& cmd)
{
    auto rpl = Message::createReply(Setup, nullType, cmd->head->reference);
    emit reply(rpl);
}

void VirtualDevice::start_logger(const std::shared_ptr< Message >& cmd)
{
    count = 0;
    logger.setInterval(cmd->getInt(0));
    logger.start();
}

void VirtualDevice::stop_logger(const std::shared_ptr< Message >& cmd)
{
    logger.stop();
}

void VirtualDevice::start_oscilloscope(const std::shared_ptr< Message >& cmd)
{
    for (auto c = 0; c < 2; ++c) {
        for (auto i = 0; i < 12; ++i) {
            auto rpl = Message::createReply(OscilloscopeData, ShortArray, 0);
            rpl->page->channel = c;
            rpl->page->index = i;
            for (auto v = 0; v < 500; ++v) {
                auto value = std::sin(2.f * pi * f * (500 * i + v) + pi * c / 2.f);
                rpl->page->values[v] = (uint16_t) ((value + 1.f) / 2.f * 4096.f);
            }
            emit reply(rpl);
        }
    }
}

} // namespace protocol
