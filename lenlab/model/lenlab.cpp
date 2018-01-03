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
    frequencysweep(new Frequencysweep(this)),
    voltmeter(new Voltmeter(this)),
    oscilloscope(new Oscilloscope(this)),
    signalgenerator(new Signalgenerator(this))
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
    return frequencysweep->active() || voltmeter->active() || oscilloscope->active();
}

Component *
Lenlab::getActiveComponent()
{
    if (frequencysweep->active())
        return frequencysweep;
    if (voltmeter->active())
        return voltmeter;
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
    //qDebug("on_reply");
    Command cmd = reply->getCommand();
    //qDebug() << cmd;

    if (cmd == startLogger)
        voltmeter->receive(reply);
    else if (cmd == startOscilloscope)
        oscilloscope->receive(reply);
    else if (cmd == stopOscilloscope)
        oscilloscope->finished(reply);
}

void
Lenlab::on_ready()
{
    qDebug("on_ready");

    frequencysweep->ready();
    voltmeter->ready();
    oscilloscope->ready();
    signalgenerator->ready();
}

} // namespace model
