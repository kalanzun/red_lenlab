/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "frequencysweep.h"

#include "lenlab.h"

#include "lenlab_version.h"

#include <QDebug>
#include <QSaveFile>

#include <complex>

namespace model {

static int p_oscilloscope_data_type_id = qRegisterMetaType<Frequencysweep::pOscilloscopeData>("pOscilloscopeData");

char const * const Frequencysweep::DELIMITER = ";";

Frequencysweep::Frequencysweep(Lenlab & lenlab, protocol::Board & board, Signalgenerator & signalgenerator)
    : Component(lenlab, board)
    , m_signalgenerator(signalgenerator)
    , m_current(new FrequencySeries)
{
    connect(&m_signalgenerator, &Signalgenerator::succeeded,
            this, &Frequencysweep::on_signalgenerator_succeeded);
    connect(&m_signalgenerator, &Signalgenerator::failed,
            this, &Frequencysweep::on_signalgenerator_failed);

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
    static QString name("ie Frequenzanalyse");
    return name;
}

QString const &
Frequencysweep::getNameAccusative() const
{
    static QString name("ie Frequenzanalyse");
    return name;
}

pSeries
Frequencysweep::getSeries() const
{
    return m_current;
}

void
Frequencysweep::start()
{
    //qDebug() << "Frequencysweep::start";

    super::start();

    m_signalgenerator.lock();

    m_current.reset(new FrequencySeries());
    emit seriesChanged(m_current);
    m_index = m_current->startIndex();

    m_signalgenerator.setAmplitude(14);
    m_signalgenerator.setFrequency(m_index);
    m_signalgenerator.setSecond(1);

    m_signalgenerator_error_counter = 0;
    if (m_signalgenerator.active()) {
        m_signalgenerator.setSine();
    } else {
        m_signalgenerator.start();
    }
}

void
Frequencysweep::on_signalgenerator_succeeded(protocol::pTask const &)
{
    //qDebug() << "Frequencysweep::on_sine";

    if (!mActive) return;

    stepTimer.start(); // wait a little for the system to settle
}

void
Frequencysweep::on_signalgenerator_failed(protocol::pTask const & task)
{
    if (!mActive) return;


    if (m_signalgenerator_error_counter < 3) {
        ++m_signalgenerator_error_counter;
        auto msg = QString("%1. Wiederhole.").arg(task->getErrorMessage());
        emit mLenlab.logMessage(msg);
        m_signalgenerator.setSine();
    } else {
        emit mLenlab.logMessage(task->getErrorMessage());
        mLenlab.reset();
    }
}

void
Frequencysweep::on_step()
{
    //qDebug() << "Frequencysweep:on_step";

    if (!mActive) return;

    if (!mBoard.isOpen()) {
        return;
    }

    if (!mBoard.isReady()) {
        return;
    }

    /*
    if (index >= 66)
        samplerate = 0;
    else if (index >= 33)
        samplerate = 2;
    else
        samplerate = 3;
    */

    m_samplerate = 0;

    QVector<uint32_t> args;
    args.append(m_samplerate);

    protocol::pTask task(new protocol::Task(::startOscilloscope));
    task->getCommand()->setUInt32Vector(args);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Frequencysweep::on_succeeded);
    connect(task.data(), &protocol::Task::failed,
            this, &Frequencysweep::on_failed);
    mBoard.queueTask(task);
}

void
Frequencysweep::on_succeeded(protocol::pTask const & task)
{
    //qDebug() << "Frequencysweep::on_succeeded";

    if (!mActive) return;

    std::array< int, m_channels > index = {0};
    pOscilloscopeData waveform(new OscilloscopeData());

    for (auto reply: task->getReplies()) {
        uint16_t *data = reply->getUInt16Buffer() + m_uint16_offset;

        std::size_t channel = reply->getHead()[2];
        //uint8_t count = buffer[2];
        //qDebug() << "receive" << count << channel << last_package;

        Q_ASSERT(channel < m_channels);

        for (int i = 0; i < reply->getUInt16BufferLength() - m_uint16_offset; ++i) {
            Q_ASSERT(index[channel] < m_length);
            waveform->at(channel).at(index[channel]) = (static_cast< double >(data[i] >> 2) / 1024.0 - 0.5) * 3.3;
            ++index[channel];
        }
    }

    if (!(index[0] == m_length && index[1] == m_length)) {
        if (m_error_counter < 3) {
            emit mLenlab.logMessage("Unvollständige Daten empfangen. Wiederhole.");
            ++m_error_counter;
            stepTimer.start();
        } else {
            emit mLenlab.logMessage("Unvollständige Daten empfangen.");
            mLenlab.reset();
        }
    } else {
        m_error_counter = 0;
        emit calculate(waveform);
    }
}

void
Frequencysweep::on_calculate(pOscilloscopeData waveform)
{
    //qDebug() << "Frequencysweep::on_calculate";

    if (!mActive) return;

    auto index = m_index;
    //auto samplerate = m_samplerate;

    ++m_index;
    if (m_index < m_current->stopIndex()) {
        m_signalgenerator.setFrequency(m_index);

        m_signalgenerator_error_counter = 0;
        m_signalgenerator.setSine();
    }
    else {
        stop();
    }

    double f = m_signalgenerator.getFrequency(index);

    std::complex<double> sum0, sum1, y;
    double value, angle, x;

    double pi = std::acos(-1);
    std::complex<double> j(0, 1);

    sum0 = 0;
    sum1 = 0;

    for (int i = 0; i < m_length; ++i) {
        //x = 2 * pi * f * 1e-6 * (1<<samplerate) * ((double) i - (waveform->at(0).size() / 2));
        x = 2 * pi * f * 1e-6 * static_cast< double >(i);
        y = std::sin(x) + j * std::cos(x);
        sum0 += y * waveform->at(0).at(i);
        sum1 += y * waveform->at(1).at(i);
    }

    std::complex<double> transfer_fct = sum1 / sum0;

    value = 20.0 * std::log10(std::abs(transfer_fct));
    angle = std::arg(transfer_fct) / pi * 180;

    if (angle > 180) angle = 360 - angle;
    if (angle < -180) angle = 360 + angle;

    //qDebug() << "frequency" << index << f;

    m_current->appendFrequency(f);
    m_current->append(0, value);
    m_current->append(1, angle);

    emit seriesUpdated(m_current);
}

void Frequencysweep::on_failed(protocol::pTask const & task)
{
    qDebug() << task->getErrorMessage();
    emit mLenlab.logMessage(task->getErrorMessage());
    mLenlab.reset();
}

void
Frequencysweep::stop()
{
    super::stop();

    // unlock first, then signalgenerator handles an error on its own
    m_signalgenerator.unlock();
    m_signalgenerator.stop();
}

void Frequencysweep::reset()
{
    super::reset();

    stepTimer.stop();
}

void
Frequencysweep::save(QTextStream &stream)
{
    stream << QString("Lenlab red %1.%2 Frequenzanalyse-Daten\n").arg(MAJOR).arg(MINOR);

    stream << "Frequenz" << DELIMITER << "Amplitude" << DELIMITER << "Phase" << "\n";

    for (int i = 0; i < m_current->getLength(); i++) {
        stream << m_current->getX(i) << DELIMITER << m_current->getY(i, 0) << DELIMITER << m_current->getY(i, 1) << "\n";
    }
}

} // namespace model
