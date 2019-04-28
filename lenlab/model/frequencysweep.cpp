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

#include "config.h"
#include "lenlab.h"

#include <QDebug>
#include <QSaveFile>

#include <complex>

namespace model {

Frequencysweep::Frequencysweep(Lenlab & lenlab, protocol::Board & board, Signalgenerator & signalgenerator)
    : Component(lenlab, board)
    , m_signalgenerator(signalgenerator)
    , m_current(new FrequencySeries())
{
    connect(&m_signalgenerator, &Signalgenerator::sine,
            this, &Frequencysweep::on_sine);

    stepTimer.setInterval(m_task_delay);
    stepTimer.setSingleShot(true);
    connect(&stepTimer, &QTimer::timeout,
            this, &Frequencysweep::on_step);

    connect(this, &Frequencysweep::calculate,
            this, &Frequencysweep::on_calculate,
            Qt::QueuedConnection);
}

QString const &
Frequencysweep::getNameNominative() const
{
    static QString name("die Frequenzanalyse");
    return name;
}

QString const &
Frequencysweep::getNameAccusative() const
{
    static QString name("die Frequenzanalyse");
    return name;
}

void
Frequencysweep::start()
{
    super::start();

    if (m_signalgenerator.active()) m_signalgenerator.stop();
    m_signalgenerator.lock();

    m_current->clear();
    m_index = m_current->start_index;

    m_signalgenerator.setAmplitude(14);
    m_signalgenerator.setFrequency(m_index);
    m_signalgenerator.setSecond(1);
    m_signalgenerator.start();
}

void
Frequencysweep::on_sine()
{
    if (!mActive) return;

    stepTimer.start(); // wait a little for the system to settle
}

void
Frequencysweep::on_step()
{
    if (!mActive) return;

    if (!mBoard.isOpen()) {
        return;
    }

    if (!mBoard.isReady()) {
        return;
    }

    uint32_t samplerate = 0;
    /*
    if (index >= 66)
        samplerate = 0;
    else if (index >= 33)
        samplerate = 2;
    else
        samplerate = 3;
    */

    m_incoming.reset(new Waveform());
    m_incoming->setSamplerate(1e6/(1<<(samplerate+2)));
    m_samplerate = samplerate;

    QVector<uint32_t> args;
    args.append(samplerate + 2);

    protocol::pMessage cmd(new protocol::Message);
    cmd->setCommand(::startOscilloscope);
    cmd->setUInt32Vector(args);

    // TODO try

    auto task = mBoard.startTask(cmd, m_task_timeout);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Frequencysweep::on_succeeded);
    connect(task.data(), &protocol::Task::failed,
            this, &Frequencysweep::on_failed);

}

void
Frequencysweep::on_succeeded(protocol::pTask const & task)
{
    if (!mActive) return;

    for (auto reply: task->getReplies()) {
        uint16_t *data = reply->getUInt16Buffer();

        uint16_t channel = data[0];//reply->getHead()[2];
        //uint8_t count = buffer[2];
        //qDebug() << "receive" << count << channel << last_package;

        for (uint32_t i = 1; i < 500; ++i) {
            m_incoming->append(channel, (((double) (data[i] >> 2)) / 1024.0 - 0.5) * 3.3);
        }

    }

    m_incoming->setView(m_incoming->getLength(0));
    emit calculate();
}

void
Frequencysweep::on_calculate()
{
    //qDebug("on_calculate");

    if (!mActive) return;

    auto index = m_index;
    auto samplerate = m_samplerate;
    auto incoming = m_incoming;

    ++m_index;
    if (m_index < m_current->stop_index) {
        m_signalgenerator.setFrequency(m_index);
        m_signalgenerator.setSine();
    }
    else {
        stop();
    }

    double f = m_signalgenerator.getFrequency(index);

    std::complex<double> sum0, sum1, y;
    double value, angle, x;

    double pi = std::acos(-1);
    std::complex<double> i(0, 1);

    sum0 = 0;
    sum1 = 0;

    incoming->setTrigger(0);

    for (uint32_t idx = 0; idx < incoming->getDataLength(0); idx++) {
        x = 2 * pi * f * 1e-6 * (1<<samplerate) * ((double) idx - (incoming->getDataLength(0) / 2));
        y = std::sin(x) + i * std::cos(x);
        sum0 += incoming->getY(idx, 0) * y;
        sum1 += incoming->getY(idx, 1) * y;
    }

    std::complex<double> transfer_fct = sum1 / sum0;
    value = 20.0 * std::log10(std::abs(transfer_fct));
    angle = std::arg(transfer_fct) / pi * 180;

    if (angle > 180) angle = 360 - angle;
    if (angle < -180) angle = 360 + angle;

    qDebug() << "frequency" << index << f;

    m_current->append(0, f);
    m_current->append(1, value);
    m_current->append(2, angle);

    emit replot();
}

void Frequencysweep::on_failed(const protocol::pTask &)
{
    Q_ASSERT(false);
}

void
Frequencysweep::stop()
{
    super::stop();

    m_signalgenerator.stop();
    m_signalgenerator.unlock();
}

QSharedPointer<FrequencySeries>
Frequencysweep::getWaveform()
{
    return m_current;
}

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

    stream << "Frequenz" << DELIMITER << "Amplitude" << DELIMITER << "Phase" << "\n";

    for (uint32_t i = 0; i < m_current->getLength(0); i++) {
        stream << m_current->getX(i) << DELIMITER << m_current->getY(i, 1) << DELIMITER << m_current->getY(i, 2) << "\n";
    }

    file.commit();
}

} // namespace model
