/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

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
