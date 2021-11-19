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

#include "oscilloscope.h"

#include "lenlab.h"
#include "utils.h"

#include "lenlab_version.h"

#include <QDebug>

namespace model {

char const * const Oscilloscope::DELIMITER = ";";

Oscilloscope::Oscilloscope(Lenlab & lenlab, protocol::Board & board)
    : Component(lenlab, board)
    , waveform(new Waveform)
{
    startTimer.setInterval(m_task_delay);
    startTimer.setSingleShot(true);
    connect(&startTimer, &QTimer::timeout,
            this, &Oscilloscope::on_start);
}

QString const &
Oscilloscope::getNameNominative() const
{
    static QString name("as Oszilloskop");
    return name;
}

QString const &
Oscilloscope::getNameAccusative() const
{
    static QString name("as Oszilloskop");
    return name;
}

pSeries
Oscilloscope::getSeries() const
{
    return waveform;
}

void
Oscilloscope::start()
{
    super::start();

    startTimer.start();
}

void
Oscilloscope::stop()
{
    super::stop();

    startTimer.stop();
}

void Oscilloscope::reset()
{
    super::reset();

    startTimer.stop();
}

void
Oscilloscope::setSamplerateIndex(int index)
{
    // save for next waveform
    m_samplerate_index = index;
}

int
Oscilloscope::samplerateIndex() const
{
    return m_samplerate_index;
}

void
Oscilloscope::setViewIndex(int index)
{
    // save for next waveform
    m_view_index = index;

    // also change current waveform
    waveform->setView(to_view(m_view_index));
    emit seriesChanged(waveform);
}

int
Oscilloscope::viewIndex() const
{
    return m_view_index;
}

QString
Oscilloscope::getSamplerateLabel(int index)
{
    return QString("%1 kHz").arg(german_double(to_samplerate(index) / 1000));
}

QString
Oscilloscope::getViewLabel(int index) const
{
    return QString("%1 ms").arg(german_double(to_view(index) / to_samplerate(m_samplerate_index) * 1000));
}

void
Oscilloscope::on_start()
{
    if (!mActive) return;

    if (!mBoard.isOpen()) {
        return;
    }

    if (!mBoard.isReady()) {
        startTimer.start();
        return;
    }

    incoming.reset(new Waveform);
    incoming->setSamplerate(to_samplerate(m_samplerate_index));

    QVector<uint32_t> args;
    args.append(to_log2oversamples(m_samplerate_index));

    protocol::pTask task(new protocol::Task(::startTrigger, m_task_timeout));
    task->getCommand()->setUInt32Vector(args);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Oscilloscope::on_succeeded);
    connect(task.data(), &protocol::Task::failed,
            this, &Oscilloscope::on_failed);
    mBoard.queueTask(task);
}


void
Oscilloscope::on_succeeded(protocol::pTask const & task)
{
    for (auto reply: task->getReplies()) {
        uint16_t * buffer = reply->getUInt16Buffer();

        uint16_t trigger = buffer[0];
        uint16_t state0 = buffer[2];
        uint16_t state1 = buffer[3];

        if (trigger > 0) incoming->setTrigger(trigger);

        incoming->append(0, to_double(state0));
        incoming->append(1, to_double(state1));

        int8_t * data = reinterpret_cast<int8_t *>(buffer + 6);

        for (int i = 1; i < reply->getUInt16BufferLength() - 6; ++i) {
            state0 += data[2*i];
            state1 += data[2*i+1];
            incoming->append(0, to_double(state0));
            incoming->append(1, to_double(state1));
        }
    }

    incoming->setView(to_view(m_view_index));

    waveform.swap(incoming);
    emit seriesChanged(incoming);

    if (mActive) {
        startTimer.start();
    }
}

void
Oscilloscope::on_failed(protocol::pTask const & task)
{
    qDebug() << task->getErrorMessage();
    emit mLenlab.logMessage(task->getErrorMessage());
    mLenlab.reset();
}

double
Oscilloscope::to_double(uint16_t state)
{
    return ((static_cast<double>(state)) / 1024.0 - 0.5) * 3.3;
}

int
Oscilloscope::to_log2oversamples(int index)
{
    return index + 2;
}

double
Oscilloscope::to_samplerate(int index)
{
    return 1e6 / (1 << (index + 2));
}

int
Oscilloscope::to_view(int index)
{
    return 8000 >> index;
}


void
Oscilloscope::save(QTextStream &stream)
{
    stream << QString("Lenlab red %1.%2 Oszilloskop-Daten\n").arg(MAJOR).arg(MINOR);

    stream << "Zeit" << DELIMITER << "Kanal_1" << DELIMITER << "Kanal_2" << "\n";

    for (int i = 0; i < waveform->getLength(); ++i) {
        stream << waveform->getX(i) << DELIMITER << waveform->getY(i, 0) << DELIMITER << waveform->getY(i, 1) << "\n";
    }
}

} // namespace model
