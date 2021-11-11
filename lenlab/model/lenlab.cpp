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

#include "lenlab.h"

#include <QDebug>

namespace model {

Lenlab::Lenlab(QObject * parent)
    : QObject(parent)
    , board()
    , voltmeter(*this, board)
    , signalgenerator(*this, board)
    , oscilloscope(*this, board)
    , frequencysweep(*this, board, signalgenerator)
{
    connect(&board, &protocol::Board::ready,
            this, &Lenlab::on_ready);
    connect(&board, &protocol::Board::log,
            this, &Lenlab::on_log);
    connect(&board, &protocol::Board::error,
            this, &Lenlab::on_error);
}

bool
Lenlab::isActive() const
{
    return frequencysweep.active() || voltmeter.active() || oscilloscope.active();
}

Component *
Lenlab::getActiveComponent()
{
    if (frequencysweep.active())
        return &frequencysweep;
    if (voltmeter.active())
        return &voltmeter;
    if (oscilloscope.active())
        return &oscilloscope;

    throw std::exception();
}

void Lenlab::reset()
{
    emit logMessage("Reset.");

    voltmeter.reset();
    signalgenerator.reset();
    oscilloscope.reset();
    frequencysweep.reset();
}

void
Lenlab::lookForBoard()
{
    board.lookForBoard();
}

void
Lenlab::on_ready()
{
    auto msg = QString("Lenlab-Board Version %1.%2 verbunden.").arg(board.getVersionMajor()).arg(board.getVersionMinor());
    emit logMessage(msg);

}

void
Lenlab::on_log(QString const & msg)
{
    emit logMessage(msg);
}

void
Lenlab::on_error(QString const & msg)
{
    emit logMessage(msg);
    reset();
}

} // namespace model
