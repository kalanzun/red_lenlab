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
public:
    explicit Component(Lenlab *parent);
    virtual ~Component();

    bool isActive();

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    virtual void start();
    virtual void stop();

    virtual void receive(const usb::pMessage &reply);
    virtual void ready();

signals:

public slots:

protected:
    Lenlab *lenlab;

    bool active = 0;

};

} // namespace model

#endif // COMPONENT_H
