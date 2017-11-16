#include "lenlab.h"

namespace model {

Lenlab::Lenlab(QObject *parent) :
    QObject(parent),
    frequency(new Frequency(this)),
    logger(new Logger(this)),
    oscilloscope(new Oscilloscope(this)),
    signal(new Signal(this))
{
    connect(this, SIGNAL(reply(usb::pMessage)),
            this, SLOT(on_reply(usb::pMessage)));
}

bool
Lenlab::isActive()
{
    return frequency->isActive() || logger->isActive() || oscilloscope->isActive();
}

Component *
Lenlab::getActiveComponent()
{
    if (frequency->isActive())
        return frequency;
    if (logger->isActive())
        return logger;
    if (oscilloscope->isActive())
        return oscilloscope;

    throw std::exception();
}

void
Lenlab::send(const usb::pMessage &cmd)
{
    emit command(cmd);
}

void
Lenlab::on_reply(const usb::pMessage &reply)
{
    qDebug("on_reply");
    Command cmd = reply->getCommand();

    if (cmd == startLogger)
        logger->receive(reply);
}

} // namespace model
