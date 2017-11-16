#include "logger.h"
#include "lenlab.h"

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
    lenlab->send();
    qDebug("start");
    super::start();
}

} // namespace model
