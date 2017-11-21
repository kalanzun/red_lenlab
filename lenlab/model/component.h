#ifndef COMPONENT_H
#define COMPONENT_H

#include "usb/message.h"
#include "usb/handler.h"
#include <QObject>

namespace model {

class Lenlab;

/**
 * @brief Base class for Lenlab components.
 */

class Component : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    explicit Component(Lenlab *parent);
    virtual ~Component();

    bool active() const;
    void setActive(bool active);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    virtual void start();
    virtual void stop();

    virtual void receive(const usb::pMessage &reply);
    virtual void ready();

signals:
    void activeChanged(bool);

public slots:

protected:
    Lenlab *lenlab;

    bool m_active = 0;

};

} // namespace model

#endif // COMPONENT_H
