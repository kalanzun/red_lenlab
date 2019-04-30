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

#include "config.h"
#include "lenlab.h"
#include "utils.h"

#include <QDebug>
#include <QSaveFile>

namespace model {

Oscilloscope::Oscilloscope(Lenlab & lenlab, protocol::Board & board)
    : Component(lenlab, board)
    , waveform(new Waveform())
    , samplerateIndex(3)
{
    double value;

    for (uint32_t i = 0; i < samplerateIndex.length; ++i) {
        value = 1000.0 / (1<<(i+2));
        samplerateIndex.labels << QString("%1 kHz").arg(german_double(value));
    }

    startTimer.setInterval(m_task_delay);
    startTimer.setSingleShot(true);
    connect(&startTimer, &QTimer::timeout,
            this, &Oscilloscope::on_start);
}

QString const &
Oscilloscope::getNameNominative() const
{
    static QString name("das Oszilloskop");
    return name;
}

QString const &
Oscilloscope::getNameAccusative() const
{
    static QString name("das Oszilloskop");
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

void
Oscilloscope::setSamplerate(uint32_t index)
{
    samplerate = index;
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

    incoming.reset(new Waveform());
    incoming->setSamplerate(1e6/(1<<(samplerate+2)));

    QVector<uint32_t> args;
    args.append(samplerate + 2);

    protocol::pMessage cmd(new protocol::Message);
    cmd->setCommand(::startTrigger);
    //cmd->setCommand(::startTriggerLinearTestData);
    cmd->setUInt32Vector(args);

    // TODO try

    auto task = mBoard.startTask(cmd, m_task_timeout);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Oscilloscope::on_succeeded);
    connect(task.data(), &protocol::Task::failed,
            this, &Oscilloscope::on_failed);
}


void
Oscilloscope::on_succeeded(protocol::pTask const & task)
{
    for (auto reply: task->getReplies()) {
        uint16_t * buffer = reply->getUInt16Buffer();

        uint16_t trigger = buffer[0];
        uint16_t state0 = buffer[2];
        uint16_t state1 = buffer[3];

        if (trigger) {
            incoming->setTrigger(trigger);
        }

        incoming->append(0, to_double(state0));
        incoming->append(1, to_double(state1));

        int8_t * data = reinterpret_cast<int8_t *>(buffer + 6);

        for (uint32_t i = 1; i < reply->getUInt16BufferLength() - 6; ++i) {
            state0 += data[2*i];
            state1 += data[2*i+1];
            incoming->append(0, to_double(state0));
            incoming->append(1, to_double(state1));
        }
    }

    incoming->setView(8000);

    waveform.swap(incoming);
    emit seriesChanged(incoming);

    if (mActive) {
        startTimer.start();
    }
}

void
Oscilloscope::on_failed(protocol::pTask const & task)
{
    Q_UNUSED(task);

    qDebug("Oscilloscope::on_failed");

    Q_ASSERT(false);
}

double
Oscilloscope::to_double(uint16_t state)
{
    return ((static_cast<double>(state)) / 1024.0 - 0.5) * 3.3;
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

    for (uint32_t i = 0; i < waveform->getLength(0); ++i) {
        stream << waveform->getX(i) << DELIMITER << waveform->getY(i, 0) << DELIMITER << waveform->getY(i, 1) << "\n";
    }

    file.commit();
}

} // namespace model
