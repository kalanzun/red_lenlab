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
Component::setBoard(const QPointer<protocol::Board> &board)
{
    this->board = board;
}

} // namespace model
