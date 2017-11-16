#include "lenlab.h"

namespace model {

Lenlab::Lenlab(QObject *parent) :
    QObject(parent),
    frequency(new Frequency(this)),
    logger(new Logger(this)),
    oscilloscope(new Oscilloscope(this)),
    signal(new Signal(this))
{

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
Lenlab::send()
{
    emit command();
}

void
Lenlab::reply()
{
    // switch reply->cmd
    logger->reply();
}

} // namespace model
