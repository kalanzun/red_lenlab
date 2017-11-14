#include "logger.h"

namespace model {

Logger::Logger(QObject *parent) : Component(parent)
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

} // namespace model
