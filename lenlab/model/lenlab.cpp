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

    connect(signalgenerator, SIGNAL(updated()),
            frequencysweep, SLOT(on_updated()));
}

Lenlab::~Lenlab()
{
    qDebug() << "~Lenlab";
}

void
Lenlab::setHandler(usb::Handler *handler)
{
    this->handler = handler;

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

bool
Lenlab::available()
{
    return m_ready && current_com.isNull();
}

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
    oscilloscope->try_to_start();
    frequencysweep->try_to_start();
}

void
Lenlab::on_ready()
{
    frequencysweep->ready();
    voltmeter->ready();
    oscilloscope->ready();
    //signalgenerator->ready();

    m_ready = 1;
}

} // namespace model
