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

#include "lenlab.h"

#include "exception.h"

#include <QDebug>

namespace model {

Lenlab::Lenlab(QObject * parent)
    : QObject(parent)
    , frequencysweep(*this)
    , voltmeter(*this)
    , oscilloscope(*this)
    , signalgenerator(*this)
{
    connect(&mFactory, &protocol::Factory::ready,
            this, &Lenlab::on_factory_ready);
    connect(&mFactory, &protocol::Factory::log,
            this, &Lenlab::on_factory_log);
    connect(&mFactory, &protocol::Factory::error,
            this, &Lenlab::on_factory_error);
    /*
    connect(signalgenerator, SIGNAL(updated()),
            frequencysweep, SLOT(on_updated()));
            */

    // TODO Factory
    /*
    connect(&manager, &protocol::Manager::error,
            this, &Lenlab::on_error);
    connect(&manager, &protocol::Manager::ready,
            this, &Lenlab::on_ready);

    manager.start();
    */
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

protocol::pBoard const &
Lenlab::board() const
{
    if (mBoard)
        return mBoard;
    else {
        throw Exception("Kein Lenlab-Board verbunden.");
    }
}

void
Lenlab::connectToBoard()
{
    mFactory.connectToBoard();
}
/*
bool
Lenlab::available()
{
    //TODO
    return 1;//return m_ready;
}
*/

void
Lenlab::on_factory_ready(protocol::pBoard const & board)
{
    mBoard = board;

    connect(mBoard.data(), &protocol::Board::error,
            this, &Lenlab::on_board_error);

    connect(mBoard.data(), &protocol::Board::logger,
            &voltmeter, &Voltmeter::on_logger);

    auto msg = QString("Lenlab-Board Version %1.%2 verbunden.").arg(board->getMajor()).arg(board->getMinor());
    emit logMessage(msg);

    /*
    frequencysweep->setBoard(board);
    voltmeter->setBoard(board);
    oscilloscope->setBoard(board);
    //signalgenerator->setBoard(board);
    */
}

void
Lenlab::on_factory_log(QString const & msg)
{
    emit logMessage(msg);
}

void
Lenlab::on_factory_error(QString const & msg)
{
    emit logMessage(msg);
}

void
Lenlab::on_board_error(QString const & msg)
{
    auto board = mBoard;
    mBoard.clear();
    connect(board.data(), &protocol::Board::destroyed,
            this, &Lenlab::on_board_destroyed);
    emit logMessage(msg);
}

void
Lenlab::on_board_destroyed()
{
    // attempt to connect again
    QTimer::singleShot(mErrorTime, this, &Lenlab::connectToBoard);
}

/*
QPointer<Communication>
Lenlab::initCommunication()
{
    if (!current_com.isNull())
        throw std::exception();

    current_com = new Communication(handler);
    connect(current_com, SIGNAL(destroyed(QObject *)),
            this, SLOT(on_comDestroyed(QObject *)));

    return current_com;
}

void
Lenlab::on_comDestroyed(QObject *obj)
{
    Q_UNUSED(obj);
    signalgenerator->try_to_setSine();
    signalgenerator->try_to_stop();
    voltmeter->try_to_start();
    oscilloscope->try_to_start();
    frequencysweep->try_to_start();
}
*/


} // namespace model
