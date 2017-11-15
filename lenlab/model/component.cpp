#include "component.h"

namespace model {

Component::Component(QObject *parent) : QObject(parent)
{

}

bool
Component::isActive()
{
    return active;
}

QString
Component::getNameNominative()
{
    return "ie Komponente";
}

QString
Component::getNameAccusative()
{
    return "ie Komponente";
}

void
Component::start()
{
    active = true;
}

void
Component::stop()
{
    active = false;
}

} // namespace model
