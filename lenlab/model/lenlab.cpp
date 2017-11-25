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
#include <QDebug>

namespace model {

Lenlab::Lenlab(QObject *parent) :
    QObject(parent),
    frequency(new Frequency(this)),
    logger(new Logger(this)),
    oscilloscope(new Oscilloscope(this)),
    signal(new Signal(this))
{
    qDebug() << "Lenlab";
}

Lenlab::~Lenlab()
{
    qDebug() << "~Lenlab";
}

void
Lenlab::setHandler(usb::Handler *handler)
{
    this->handler = handler;

    connect(handler, SIGNAL(reply(usb::pMessage)),
            this, SLOT(on_reply(usb::pMessage)));
    connect(handler, SIGNAL(ready()),
            this, SLOT(on_ready()));
}

bool
Lenlab::isActive()
{
    return frequency->active() || logger->active() || oscilloscope->active();
}

Component *
Lenlab::getActiveComponent()
{
    if (frequency->active())
        return frequency;
    if (logger->active())
        return logger;
    if (oscilloscope->active())
        return oscilloscope;

    throw std::exception();
}

void
Lenlab::send(const usb::pMessage &cmd)
{
    handler->send(cmd);
}

void
Lenlab::on_reply(const usb::pMessage &reply)
{
    qDebug("on_reply");
    Command cmd = reply->getCommand();

    if (cmd == startLogger)
        logger->receive(reply);
}

void
Lenlab::on_ready()
{
    qDebug("on_ready");

    frequency->ready();
    logger->ready();
    oscilloscope->ready();
    signal->ready();
}

} // namespace model
