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
#include "utils.h"
#include <QDebug>
#include <QSaveFile>

namespace model {

Oscilloscope::Oscilloscope(Lenlab *parent) : Component(parent), samplerateIndex(3)
{
    double value;

    for (uint32_t i = 0; i < samplerateIndex.length; i++) {
        value = 1000.0 / (1<<(i+2));
        samplerateIndex.labels << QString("%1 kHz").arg(german_double(value));
    }

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

    qDebug() << "stop";

    pending = 0;
}

void
Oscilloscope::try_to_start()
{
    if (pending && lenlab->available()) {
        pending = 0;
        restart();
    }
}

void
Oscilloscope::restart()
{
    incoming.reset(new Waveform());
    incoming->setSamplerate(1e6/(1<<(samplerate+2)));

    auto transaction = board->startOscilloscopeTrigger(samplerate+2);
    connect(transaction.data(), &protocol::Transaction::succeeded,
            this, &Oscilloscope::on_succeeded);
}

void
Oscilloscope::setSamplerate(uint32_t index)
{
    samplerate = index;
}

void
Oscilloscope::on_succeeded(const protocol::pMessage &reply)
{
    Q_UNUSED(reply)

    auto transaction = qobject_cast<protocol::Transaction *>(QObject::sender());

    for (auto reply: transaction->replies) {
        uint8_t *buffer = reply->getUInt8Buffer();

        uint16_t state0 = *reinterpret_cast<uint16_t *>(buffer + 0);
        uint16_t state1 = *reinterpret_cast<uint16_t *>(buffer + 2);
        uint16_t trigger = *reinterpret_cast<uint16_t *>(buffer + 4);

        if (trigger) {
            incoming->setTrigger(trigger);
        }

        incoming->append(0, ((static_cast<double>(state0)) / 1024.0 - 0.5) * 3.3);
        incoming->append(1, ((static_cast<double>(state1)) / 1024.0 - 0.5) * 3.3);

        int8_t *data = reinterpret_cast<int8_t *>(buffer + 8);

        for (uint32_t i = 1; i < 500; ++i) {
            state0 += data[2*i];
            state1 += data[2*i+1];
            incoming->append(0, ((static_cast<double>(state0)) / 1024.0 - 0.5) * 3.3);
            incoming->append(1, ((static_cast<double>(state1)) / 1024.0 - 0.5) * 3.3);
        }
    }

    incoming->setView(8000);

    waveform.swap(incoming);
    emit replot();

    if (m_active) {
        pending = 1;
        try_to_start();
    }
}

void
Oscilloscope::save(const QString &fileName)
{
    QSaveFile file(fileName);
    qDebug("save");

    if (!file.open(QIODevice::WriteOnly)) {
        throw std::exception();
    }

    QTextStream stream(&file);

    stream << QString("Lenlab red %1.%2 Oszilloskop-Daten\n").arg(MAJOR).arg(MINOR);

    stream << "Zeit" << DELIMITER << "Kanal_1" << DELIMITER << "Kanal_2" << "\n";

    for (uint32_t i = 0; i < waveform->getLength(0); i++) {
        stream << waveform->getX(i) << DELIMITER << waveform->getY(i, 0) << DELIMITER << waveform->getY(i, 1) << "\n";
    }

    file.commit();
}

} // namespace model
