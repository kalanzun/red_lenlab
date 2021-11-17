/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "component.h"

#include "lenlab.h"

#include <QDebug>

namespace model {

Component::Component(Lenlab & lenlab, protocol::Board & board)
    : QObject()
    , mLenlab(lenlab)
    , mBoard(board)
{

}

void
Component::setActive(bool active)
{
    if (mActive != active) {
        mActive = active;
        emit activeChanged(mActive);
    }
}

bool
Component::active() const
{
    return mActive;
}

QString const &
Component::getNameNominative() const
{
    static QString name("ie Komponente");
    return name;
}

QString const &
Component::getNameAccusative() const
{
    static QString name("ie Komponente");
    return name;
}

pSeries
Component::getSeries() const
{
    return pSeries();
}

void
Component::start()
{
    Q_ASSERT_X(!mLenlab.isActive(), "Component::start()", "No component may be active.");
    emit mLenlab.logMessage(QString("D%1 startet.").arg(getNameNominative()));
    setActive(true);
}

void
Component::stop()
{
    Q_ASSERT_X(mActive, "Component::stop()", "This component was not active.");
    emit mLenlab.logMessage(QString("D%1 stoppt.").arg(getNameNominative()));
    setActive(false);
}

void
Component::reset()
{
    setActive(false);
}

} // namespace model
