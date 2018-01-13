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
    //qDebug("restart");
    t = 0;
    for (auto &vector : data[write]) vector.clear();
    trigger = 0;
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
    if (channel == 0) {
        //data[write][0].append((double) t++); // x axis
        if (*(uint16_t *) (buffer + 4)) {
            trigger = *(uint16_t *) (buffer + 4);
            //qDebug() << "trigger" << trigger;
            //data[write][0].remove(0, trigger);
            Q_ASSERT(data[write][1].size() > trigger);
            Q_ASSERT(data[write][2].size() > trigger);
            data[write][1].remove(0, trigger);
            data[write][2].remove(0, trigger);
        }
    }

    data[write][1+channel].append((((double) state) / 512.0 - 1.0) * 3.3);

    //Q_ASSERT(reply->getPayloadLength() == 4 * data.size());

    // sine values
    for (uint32_t i = 1; i < 1000; i+=1) {
        //if (channel == 0)
        //    data[write][0].append((double) t++); // x axis
        //a += buffer[i];
        //data[write][1+channel].append((double) payload[i]);// / (double) 2048.0); // y axis
        state += payload[i];
        data[write][1+channel].append((((double) state) / 512.0 - 1.0) * 3.3);
        //data[write][1+channel].append((double) (state + *(int8_t *) (buffer+6+i)));// / (double) 2048.0); // y axis
        //state = state + *(int8_t *) (buffer+6+i);
        //data[write][2].append((double) buffer[6+i+1]);// / (double) 2048.0); // y axis
    }

    if (reply->getBody()[1]) { // last package
        //data[write][0].remove(data[write][0].size()-1000+trigger, 1000-trigger);
        //qDebug() << "got" << data[write][1].size() << "and" << data[write][2].size() << "samples";
        Q_ASSERT(data[write][1].size() > (1000-trigger));
        Q_ASSERT(data[write][2].size() > (1000-trigger));
        data[write][1].remove(data[write][1].size()-1000+trigger, 1000-trigger);
        data[write][2].remove(data[write][2].size()-1000+trigger, 1000-trigger);

        for (int32_t i = 0; i < data[write][1].size(); i++)
            data[write][0].append((double) i);

        read = write; // gui reads from this one
        write = (write + 1) % 2;
        emit replot();
        if (running)
            //stop();
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
