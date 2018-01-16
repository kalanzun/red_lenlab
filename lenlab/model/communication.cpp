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

#include "communication.h"
#include <QDebug>

namespace model {

Communication::Communication(usb::Handler *handler, QObject *parent) : QObject(parent), handler(handler)
{
    connect(handler, SIGNAL(reply(usb::pMessage)),
            this, SLOT(on_reply(usb::pMessage)));
}

void
Communication::send(const usb::pMessage &cmd)
{
    qDebug() << "send" << cmd;
    handler->send(cmd);
}

void
Communication::on_reply(const usb::pMessage &rpl)
{
    qDebug() << "reply" << rpl;
    emit reply(pCommunication(this), rpl);
}

} // namespace model
