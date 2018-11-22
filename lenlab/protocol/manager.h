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

#ifndef MANAGER_H
#define MANAGER_H

#include "board.h"
#include "usb/bus.h"
#include <QPointer>
#include <QTimerEvent>
#include <QObject>

namespace protocol {

/*!
 * \brief The Manager class
 *
 * Single instance only.
 *
 * Ownership: member
 */
class Manager : public QObject
{
    Q_OBJECT

    usb::Bus bus;

    QPointer<Board> board;

public:
    /*!
     * \brief Manager
     * \param parent
     *
     * May throw.
     */
    explicit Manager(QObject *parent = nullptr);

    /*!
     * \brief start
     *
     * Start looking for the board. Emit signals error or ready.
     */
    void start();

signals:
    void error(const QString &);
    void ready(const QPointer<Board> &);

private slots:
    void on_error();
    void on_init(const pMessage &);
    void on_name(const pMessage &);
    void on_version(const pMessage &);

private:
    void timerEvent(QTimerEvent *);
};

} // namespace protocol

#endif // MANAGER_H
