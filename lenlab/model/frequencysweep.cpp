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

Frequencysweep::Frequencysweep(Lenlab *parent) : Component(parent), current(new Waveform(1))
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
        divider = 3;

    lenlab->oscilloscope->setSamplerateDivider(divider);
    startTimer(10);
}

void
Frequencysweep::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    lenlab->signalgenerator->setSine(index);
}

void
Frequencysweep::receive(const usb::pMessage &reply)
{
    if (reply->getReply() == SignalSine) {
        lenlab->oscilloscope->restart();
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

    double f = lenlab->signalgenerator->getFrequency(current_index) * (1 - 0.089);
    // for some reason, the frequency is 8.9% off

    std::complex<double> sum0, sum1, y;
    double value, x;

    double pi = std::acos(-1);

    sum0 = 0;
    sum1 = 0;

    waveform->setTrigger(1000);

    for (uint32_t i = 0; i < 6000; i++) {
        x = 2 * pi * f * 1e-6 * (1<<current_divider) * ((double) i - 3000);
        y = std::sin(x) + 1i * std::cos(x);
        sum0 += waveform->getValue(0, i) * y;
        sum1 += waveform->getValue(1, i) * y;
    }

    value = std::abs(sum1) / std::abs(sum0);

    qDebug() << "frequency sweep" << current_index << value << std::abs(sum1) / 6000 << std::abs(sum0) / 6000;

    current->append(0, value);
    emit replot();
}

QSharedPointer<Waveform>
Frequencysweep::getWaveform()
{
    return current;
}


} // namespace model
