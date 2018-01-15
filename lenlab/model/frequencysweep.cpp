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

    index = 0;
    current->clear();

    restart();
}

void
Frequencysweep::stop()
{
    super::stop();
}

void
Frequencysweep::restart()
{
    if (!m_active)
        return;

    if (index >= 66)
        divider = 1;
    else if (index >= 33)
        divider = 2;
    else
        divider = 4;

    lenlab->oscilloscope->setSamplerateDivider(divider);
    toggle = 0;
    startTimer(10);
}

void
Frequencysweep::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    if (toggle) {
        lenlab->oscilloscope->restart();
    }
    else {
        lenlab->signalgenerator->setFrequency(index);
    }
}

void
Frequencysweep::receive(const usb::pMessage &reply)
{
    if (reply->getReply() == SignalSine) {
        toggle = 1;
        startTimer(100);
    }
}

void
Frequencysweep::on_replot()
{
    // Ignore normal oscilloscope events
    if (!m_active)
        return;

    auto waveform = lenlab->oscilloscope->getWaveform();

    auto current_divider = divider;
    auto current_index = index;

    index++;
    if (index < 100) {
        restart();
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

    waveform->setTrigger(1000);

    for (uint32_t idx = 0; idx < 6000; idx++) {
        x = 2 * pi * f * 1e-6 * (1<<current_divider) * ((double) idx - 3000);
        y = std::sin(x) + i * std::cos(x);
        sum0 += waveform->getY(idx, 0) * y;
        sum1 += waveform->getY(idx, 1) * y;
    }

    value = std::abs(sum1) / std::abs(sum0);
    angle = (std::arg(sum1) - std::arg(sum0)) / pi * 180;
    if (angle > 180) angle = 360 - angle;
    if (angle < -180) angle = 360 + angle;

    qDebug() << "frequency sweep" << current_index << value << std::abs(sum1) / 6000 << std::abs(sum0) / 6000 << angle << std::arg(sum0) / pi * 180 << std::arg(sum1) / pi * 180;

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
