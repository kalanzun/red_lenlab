/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#ifndef COMPONENT_H
#define COMPONENT_H

#include "series.h"

#include "protocol/board.h"

#include <QObject>
#include <QSharedPointer>

namespace model {

class Lenlab;

class Component : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

protected:
    Lenlab & mLenlab;
    protocol::Board & mBoard;

    bool mActive = 0;

public:
    explicit Component(Lenlab & lenlab, protocol::Board & board);
    Component(Component const &) = delete;

    Component & operator=(Component const &) = delete;

    bool active() const;
    void setActive(bool active);

    virtual QString const & getNameNominative() const;
    virtual QString const & getNameAccusative() const;

    virtual pSeries getSeries() const;

    virtual void start();
    virtual void stop();

    virtual void reset();

signals:
    void activeChanged(bool);

    void seriesChanged(pSeries const &);
    void seriesUpdated(pSeries const &);
};

} // namespace model

#endif // COMPONENT_H
