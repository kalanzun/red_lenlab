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

#ifndef LENLAB_H
#define LENLAB_H

#include "frequencysweep.h"
#include "voltmeter.h"
#include "oscilloscope.h"
#include "signalgenerator.h"

#include "protocol/board.h"
#include "protocol/factory.h"
#include "protocol/message.h"

#include <QObject>

namespace model {

class Lenlab : public QObject
{
    Q_OBJECT

    static int const mErrorTime = 3000;

    protocol::Factory mFactory;
    protocol::pBoard mBoard;

public:
    Frequencysweep frequencysweep;
    Voltmeter voltmeter;
    Oscilloscope oscilloscope;
    Signalgenerator signalgenerator;

    explicit Lenlab(QObject * parent = nullptr);
    Lenlab(Lenlab const & other) = delete;

    Lenlab & operator=(Lenlab const & other) = delete;

    bool isActive() const;
    Component *getActiveComponent();

    //bool available();

signals:
    void logMessage(QString const &);
    //void receive(const protocol::pMessage &);


    //void replot();

public slots:
    void connectToBoard();

private slots:
    void on_factory_ready(protocol::pBoard const &);
    void on_factory_log(QString const &);
    void on_factory_error(QString const &);
    void on_board_error(QString const &);
    void on_board_destroyed();

};

} // namespace model

#endif // LENLAB_H
