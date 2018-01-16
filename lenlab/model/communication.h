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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "usb/handler.h"
#include "usb/message.h"
#include <QObject>
#include <QPointer>

namespace model {

class Communication;

typedef QPointer<Communication> pCommunication;

class Communication : public QObject
{
    Q_OBJECT

public:
    explicit Communication(usb::Handler *handler, QObject *parent = nullptr);

    void send(const usb::pMessage &cmd);

signals:
    void reply(const pCommunication &, const usb::pMessage &);

public slots:
    void on_reply(const usb::pMessage &rpl);

private:
    usb::Handler *handler;

};

} // namespace model

#endif // COMMUNICATION_H
