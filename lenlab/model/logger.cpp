#include "logger.h"
#include "lenlab.h"

#define MSEC (1000.0)
#define VOLT (4096.0 / 3.3)

namespace model {

Logger::Logger(Lenlab *parent) : Component(parent)
{

}

QString
Logger::getNameNominative()
{
    return "er Logger";
}

QString
Logger::getNameAccusative()
{
    return "en Logger";
}

uint32_t
Logger::getInterval() const
{
    return interval;
}

void
Logger::setInterval(uint32_t interval)
{
    this->interval = interval;
}

void
Logger::start()
{
    qDebug("set intervall");
    usb::pMessage cmdSetInterval(new usb::Message());
    cmdSetInterval->setCommand(setLoggerInterval);
    *((uint32_t *) cmdSetInterval->getPayload()) = interval;
    cmdSetInterval->setPayloadLength(4);
    lenlab->send(cmdSetInterval);

    qDebug("start");
    usb::pMessage cmdStart(new usb::Message());
    cmdStart->setCommand(startLogger);
    lenlab->send(cmdStart);

    super::start();
}

void
Logger::receive(const usb::pMessage &reply)
{
    uint32_t *buffer = (uint32_t *) reply->getPayload();

    Q_ASSERT(reply->getPayloadLength() == 4 * data.size());

    // 4 bytes timestamp
    data[0].append((double) *buffer / MSEC);

    // 4 channels, 4 bytes each
    for (size_t i = 1; i < data.size(); i++) {
        data[i].append((double) buffer[i] / (double) reply->getHeader() / VOLT);
    }

    emit lenlab->replot();
}

} // namespace model
