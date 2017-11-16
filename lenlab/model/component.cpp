#include "component.h"
#include "lenlab.h"

namespace model {

Component::Component(Lenlab *parent) : QObject(parent), lenlab(parent)
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
    Q_ASSERT_X(!lenlab->isActive(), "Component::start()", "No component may be active.");
    active = true;
}

void
Component::stop()
{
    Q_ASSERT_X(active, "Component::stop()", "This component was not active.");
    active = false;
}

void
Component::receive(const usb::pMessage &reply)
{

}

} // namespace model
