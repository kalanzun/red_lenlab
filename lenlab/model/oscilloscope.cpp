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

    qDebug() << "start";

    pending = 1;
    try_to_start();
}

void
Oscilloscope::stop()
{
    super::stop();

    pending = 0;
}

void
Oscilloscope::try_to_start()
{
    if (pending && lenlab->available()) {
        restart();
        pending = 0;
    }
}

void
Oscilloscope::restart()
{
    qDebug() << "restart";

    incoming.reset(new Waveform());

    auto com = lenlab->initCommunication();
    connect(com, SIGNAL(reply(pCommunication, usb::pMessage)),
            this, SLOT(on_reply(pCommunication, usb::pMessage)));
    com->send(usb::newCommand(startOscilloscope));
}

void
Oscilloscope::setSamplerateDivider(uint8_t divider)
{
    auto cmd = usb::newCommand(setOscilloscopeSamplerateDivider);
    cmd->setByte(divider);
    //lenlab->send(cmd);
}

void
Oscilloscope::on_reply(const pCommunication &com, const usb::pMessage &reply)
{
    //qDebug("receive");
    uint8_t *buffer = reply->getBody();
    int16_t *data = (int16_t *) (reply->getBody() + 22);

    uint8_t channel = buffer[0];
    uint8_t last_package = buffer[1];
    uint8_t count = buffer[2];
    //qDebug() << count << channel << last_package;

    for (uint32_t i = 1; i < 500; i++) {
        incoming->append(channel, (((double) (data[i] >> 2)) / 1024.0 - 0.5) * 3.3);
    }

    if (last_package) {
        //qDebug() << "last package" << incoming->getLength(0) << incoming->getLength(1);

        incoming->setView(incoming->getLength(0));

        current.swap(incoming);
        incoming.clear();
        emit replot();

        com->deleteLater();

        if (m_active) {
            pending = 1;
            // try_to_start(); // does not succeed because of deleteLater()
        }
    }

    /*
    // ByteArray Code

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

    incoming->append(channel, (((double) state) / 1024.0 - 0.5) * 3.3);

    for (uint32_t i = 1; i < 1000; i++) {
        state += data[i];
        incoming->append(channel, (((double) state) / 1024.0 - 0.5) * 3.3);
    }

    if (last_package) {
        //qDebug() << "last package" << incoming->getDataLength();

        incoming->setView(6000);

        current.swap(incoming);
        incoming.clear();
        emit replot();

        if (m_active) {
            com->deleteLater();

            pending = 1;
            // try_to_start(); // does not succeed because of deleteLater()
        }
    }
    */
}

QSharedPointer<Waveform>
Oscilloscope::getWaveform()
{
    return current;
}

} // namespace model
