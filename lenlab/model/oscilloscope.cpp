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

#include "oscilloscope.h"
#include "lenlab.h"
#include "config.h"
#include <QDebug>

namespace model {

Oscilloscope::Oscilloscope(Lenlab *parent) : Component(parent)
{

}

QString
Oscilloscope::getNameNominative()
{
    return "as Oszilloskop";
}

QString
Oscilloscope::getNameAccusative()
{
    return "as Oszilloskop";
}

void
Oscilloscope::start()
{
    super::start();

    running = 1;

    restart();
}

void
Oscilloscope::stop()
{
    super::stop();

    running = 0;
}

void
Oscilloscope::restart()
{
    //qDebug() << "start";
    incoming.reset(new Waveform());
    lenlab->send(usb::newCommand(startOscilloscope));
}

void
Oscilloscope::clear()
{


}

void
Oscilloscope::setSamplerateDivider(uint8_t divider)
{
    auto cmd = usb::newCommand(setOscilloscopeSamplerateDivider);
    cmd->setByte(divider);
    lenlab->send(cmd);
}


void
Oscilloscope::receive(const usb::pMessage &reply)
{
    //qDebug("receive");

    uint8_t *buffer = reply->getBody();
    int8_t *data = (int8_t *) (reply->getBody() + 6);

    uint8_t channel = buffer[0];
    uint8_t last_package = buffer[1];

    uint16_t state = *(uint16_t *) (buffer + 2);
    uint16_t trigger = *(uint16_t *) (buffer + 4);

    if (channel == 0 && trigger) {
        //qDebug() << "trigger" << trigger;
        incoming->setTrigger(trigger);
    }

    incoming->append(channel, state);

    for (uint32_t i = 1; i < 1000; i++) {
        state += data[i];
        incoming->append(channel, state);
    }

    if (last_package) {
        //qDebug() << "last package" << incoming->getDataLength();

        current.swap(incoming);
        incoming.clear();
        emit replot();

        if (running)
            //stop();
            restart();
    }
}

QSharedPointer<Waveform>
Oscilloscope::getWaveform()
{
    return current;
}

/*
void
Oscilloscope::finished(const usb::pMessage &reply)
{
    //qDebug("finished");
    if (running)
        restart();
}
*/
/*
MinMaxVector *
Oscilloscope::getTime()
{
    return &data[read][0];
}

MinMaxVector *
Oscilloscope::getChannel(uint32_t channel)
{
    return &data[read][1+channel];
}
*/
} // namespace model
