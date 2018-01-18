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

#include "frequencysweep.h"
#include "lenlab.h"
#include <QDebug>
#include <QTimerEvent>
#include <complex>

namespace model {

Frequencysweep::Frequencysweep(Lenlab *parent) : Component(parent), current(new FrequencySeries())
{
    connect(this, SIGNAL(calculate()),
            this, SLOT(on_calculate()),
            Qt::QueuedConnection);

}

QString
Frequencysweep::getNameNominative()
{
    return "ie Frequenzanalyse";
}

QString
Frequencysweep::getNameAccusative()
{
    return "ie Frequenzanalyse";
}

void
Frequencysweep::start()
{
    super::start();

    pending = 0;
    wait_for_update = 0;

    index = 0;
    current->clear();
    lenlab->signalgenerator->setAmplitude(14);
    lenlab->signalgenerator->setDivider(1);

    step();
}

void
Frequencysweep::stop()
{
    super::stop();

    pending = 0;
    wait_for_update = 0;
}

void
Frequencysweep::step()
{
    if (!m_active)
        return;

    qDebug() << "step";

    wait_for_update = 1;
    lenlab->signalgenerator->setFrequency(index);
}

void
Frequencysweep::on_updated()
{
    if (!wait_for_update)
        return;

    pending = 1;
    wait_for_update = 0;
}

void
Frequencysweep::try_to_start()
{
    if (pending && lenlab->available()) {
        pending = 0;
        restart();
    }
}

void
Frequencysweep::restart()
{
    qDebug() << "frequencysweep restart";

    incoming.reset(new Waveform());

    if (index >= 66)
        samplerate = 0;
    else if (index >= 33)
        samplerate = 2;
    else
        samplerate = 3;

    auto com = lenlab->initCommunication();
    connect(com, SIGNAL(reply(pCommunication, usb::pMessage)),
            this, SLOT(on_reply(pCommunication, usb::pMessage)));
    auto cmd = usb::newCommand(startOscilloscope);
    cmd->setBodyLength(0);
    cmd->setType(IntArray);
    cmd->setInt(samplerate);
    com->send(cmd);
}

void
Frequencysweep::on_reply(const pCommunication &com, const usb::pMessage &reply)
{
    //qDebug("receive");
    uint8_t *buffer = reply->getBody();
    int16_t *data = (int16_t *) (reply->getBody() + 22);

    uint8_t channel = buffer[0];
    uint8_t last_package = buffer[1];
    uint8_t count = buffer[2];
    qDebug() << "receive" << count << channel << last_package;

    for (uint32_t i = 1; i < 500; i++) {
        incoming->append(channel, (((double) (data[i] >> 2)) / 1024.0 - 0.5) * 3.3);
    }

    if (last_package) {
        qDebug() << "last package" << incoming->getLength(0) << incoming->getLength(1);

        //incoming->setView(incoming->getLength(0));

        com->deleteLater();
        emit calculate();

    }

}

void
Frequencysweep::on_calculate()
{
    qDebug("on_calculate");

    if (!m_active)
        return;

    auto current_samplerate = samplerate;
    auto current_index = index;

    index++;
    if (index < 100) {
        step();
    }
    else {
        stop();
    }

    double f = lenlab->signalgenerator->getFrequency(current_index);

    std::complex<double> sum0, sum1, y;
    double value, angle, x;

    double pi = std::acos(-1);
    std::complex<double> i(0, 1);

    sum0 = 0;
    sum1 = 0;

    incoming->setTrigger(0);

    for (uint32_t idx = 0; idx < incoming->getDataLength(0); idx++) {
        x = 2 * pi * f * 1e-6 * (1<<current_samplerate) * ((double) idx - (incoming->getDataLength(0) / 2));
        y = std::sin(x) + i * std::cos(x);
        sum0 += incoming->getY(idx, 0) * y;
        sum1 += incoming->getY(idx, 1) * y;
    }

    value = 20.0 * std::log10(std::abs(sum1) / std::abs(sum0));
    angle = (std::arg(sum1) - std::arg(sum0)) / pi * 180;
    if (angle > 180) angle = 360 - angle;
    if (angle < -180) angle = 360 + angle;

    //qDebug() << "frequency sweep" << current_index << value << std::abs(sum1) / incoming->getLength(0) << std::abs(sum0) / incoming->getLength(0) << angle << std::arg(sum0) / pi * 180 << std::arg(sum1) / pi * 180;

    current->append(0, value);
    current->append(1, angle);

    emit replot();
}

QSharedPointer<FrequencySeries> Frequencysweep::getWaveform()
{
    return current;
}

/*
void
Frequencysweep::save(const QString &fileName)
{
    QSaveFile file(fileName);
    qDebug("save");

    if (!file.open(QIODevice::WriteOnly)) {
        throw std::exception();
    }

    QTextStream stream(&file);

    stream << QString("Lenlab red %1.%2 Frequenzanalyse-Daten\n").arg(MAJOR).arg(MINOR);

    stream << "Frequenz" << DELIMITER << "Betrag" << DELIMITER << "Winkel" << "\n";

    for (uint8_t i = 0; i < current->getLength(); i++) {
        stream << lenlab->signalgenerator->getFrequency(i) << DELIMITER << current->getValue(0, i) << DELIMITER << current->getValue(1, i) << "\n";
    }

    file.commit();
}
*/

} // namespace model
