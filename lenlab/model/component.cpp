#include "component.h"
#include "lenlab.h"
#include <QDebug>

namespace model {

Component::Component(Lenlab *parent) : QObject(parent), lenlab(parent)
{
    qDebug() << "Component";
}

Component::~Component()
{
    qDebug() << "~Component";
}

void
Component::setActive(bool active)
{
    if (m_active != active)
    {
        m_active = active;
        emit activeChanged(m_active);
    }
}

bool
Component::active() const
{
    return m_active;
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
    setActive(true);
}

void
Component::stop()
{
    Q_ASSERT_X(m_active, "Component::stop()", "This component was not active.");
    setActive(false);
}

void
Component::receive(const usb::pMessage &reply)
{

}

void
Component::ready()
{

}

} // namespace model
