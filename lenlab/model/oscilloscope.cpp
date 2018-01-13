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
    qDebug("restart");
    t = 0;
    for (auto &vector : data[write]) vector.clear();
    lenlab->send(usb::newCommand(startOscilloscope));
}

void
Oscilloscope::clear()
{


}

void
Oscilloscope::receive(const usb::pMessage &reply)
{
    //qDebug("receive");

    uint8_t *buffer = reply->getBody();
    int8_t *payload = (int8_t *) (reply->getBody() + 6);

    uint8_t channel = buffer[0];

    uint16_t state = *(uint16_t *) (buffer + 2);
    if (channel == 0)
        data[write][0].append((double) t++); // x axis
    data[write][1+channel].append((double) state);

    //Q_ASSERT(reply->getPayloadLength() == 4 * data.size());

    // sine values
    for (uint32_t i = 1; i < 1000; i+=1) {
        if (channel == 0)
            data[write][0].append((double) t++); // x axis
        //a += buffer[i];
        //data[write][1+channel].append((double) payload[i]);// / (double) 2048.0); // y axis
        state += payload[i];
        data[write][1+channel].append((double) state);
        //data[write][1+channel].append((double) (state + *(int8_t *) (buffer+6+i)));// / (double) 2048.0); // y axis
        //state = state + *(int8_t *) (buffer+6+i);
        //data[write][2].append((double) buffer[6+i+1]);// / (double) 2048.0); // y axis
    }

    if (reply->getBody()[1]) { // last package
        read = write; // gui reads from this one
        write = (write + 1) % 2;
        emit replot();
        if (running)
            restart();
    }

}

void
Oscilloscope::finished(const usb::pMessage &reply)
{
    //qDebug("finished");
    if (running)
        restart();
}

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

} // namespace model
