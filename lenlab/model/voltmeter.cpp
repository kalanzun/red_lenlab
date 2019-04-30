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

#include "voltmeter.h"

#include "config.h"
#include "lenlab.h"

#include <QDebug>
#include <QIODevice>
#include <QSaveFile>
#include <QTextStream>

namespace model {

double const Voltmeter::MSEC = 1000.0;

double const Voltmeter::VOLT = 4096.0 / 3.3;

Voltmeter::Voltmeter(Lenlab &lenlab, protocol::Board &board)
    : Component(lenlab, board)
    , m_loggerseries(new Loggerseries)
{
    connect(&mAutoSaveTimer, &QTimer::timeout,
            this, &Voltmeter::on_autosave);
    mAutoSaveTimer.setInterval(3000);

    connect(&mBoard, &protocol::Board::logger_data,
            this, &Voltmeter::on_logger_data);
}

QString const &
Voltmeter::getNameNominative() const
{
    static QString name("das Spannungsmessgerät");
    return name;
}

QString const &
Voltmeter::getNameAccusative() const
{
    static QString name("das Spannungsmessgerät");
    return name;
}

void
Voltmeter::setMeasurementData(bool measurementData)
{
    if (mMeasurementData != measurementData)
    {
        mMeasurementData = measurementData;
        emit measurementDataChanged(mMeasurementData);
    }
}

bool
Voltmeter::measurementData() const
{
    return mMeasurementData;
}

void
Voltmeter::setUnsavedData(bool unsavedData)
{
    if (mUnsavedData != unsavedData)
    {
        mUnsavedData = unsavedData;
        emit unsavedDataChanged(mUnsavedData);
    }
}

bool
Voltmeter::unsavedData() const
{
    return mUnsavedData;
}

void
Voltmeter::setAutoSave(bool autoSave)
{
    if (mAutoSave != autoSave) {
        mAutoSave = autoSave;
        if (mAutoSave) {
            Q_ASSERT_X(!mFileName.isEmpty(), "Voltmeter::setAutoSave()", "No fileName set");
            mAutoSaveTimer.start();
        } else {
            mAutoSaveTimer.stop();
        }
        emit autoSaveChanged(mAutoSave);
    }
}

bool
Voltmeter::autoSave() const
{
    return mAutoSave;
}

void
Voltmeter::setFileName(const QString &fileName)
{
    if (mFileName != fileName) {
        mFileName = fileName;
        emit fileNameChanged(mFileName);
    }
}

const QString &
Voltmeter::fileName() const
{
    return mFileName;
}

void
Voltmeter::setChannels(const std::bitset<4> &channels)
{
    if (mChannels != channels) {
        mChannels = channels;
        emit channelsChanged(mChannels);
    }
}

const std::bitset<4> &
Voltmeter::channels() const
{
    return mChannels;
}

void
Voltmeter::setInterval(uint32_t interval)
{
    if (mInterval != interval) {
        mInterval = interval;
        emit intervalChanged(interval);
    }
}

uint32_t
Voltmeter::interval() const
{
    return mInterval;
}

pSeries
Voltmeter::getSeries() const
{
    return m_loggerseries;
}

void
Voltmeter::start()
{
    if (mActive) return;

    super::start();

    qDebug("start");

    // TODO interval locking
    m_loggerseries->setInterval(mInterval);

    QVector<uint32_t> args;
    args.append(m_loggerseries->interval());

    protocol::pMessage cmd(new protocol::Message());
    cmd->setCommand(::startLogger);
    cmd->setUInt32Vector(args);

    auto task = mBoard.startTask(cmd);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Voltmeter::on_start);
    connect(task.data(), &protocol::Task::failed,
            this, &Voltmeter::on_start_failed);
}

void
Voltmeter::stop()
{
    if (!mActive) return;

    protocol::pMessage cmd(new protocol::Message());
    cmd->setCommand(::stopLogger);

    auto task = mBoard.startTask(cmd);
    connect(task.data(), &protocol::Task::succeeded,
            this, &Voltmeter::on_stop);
    connect(task.data(), &protocol::Task::failed,
            this, &Voltmeter::on_stop_failed);
}

/*
void
Voltmeter::receive(const usb::pMessage &reply)
{
    Q_UNUSED(reply);

    qDebug("receive");

    //uint32_t *buffer = (uint32_t *) reply->getBody();

    //Q_ASSERT(reply->getBodyLength() == 4 * data.size());

    // 4 bytes timestamp
    //data[0].append(time_offset + (double) *buffer / MSEC);

    // 4 channels, 4 bytes each
    // old comment!
    //for (size_t i = 1; i < data.size(); i++) {
    //    data[i].append((double) buffer[i] / (double) reply->getBody()[0] / VOLT); // TODO Fix for new communictaion
    //}

    setMeasurementData(true);
    setUnsavedData(true);

    emit replot();
}
*/
/*
void
Voltmeter::ready()
{
    if (m_active)
        restart();
}
*/

void
Voltmeter::clear()
{
    if (active())
        stop();

    setAutoSave(false);
    setFileName(QString());
    setUnsavedData(false);
    setMeasurementData(false);

    m_loggerseries.reset(new Loggerseries);
    emit seriesChanged(m_loggerseries);
}

void
Voltmeter::save(const QString &fileName)
{
    setAutoSave(false);
    setFileName(fileName);
    do_save();
}

void
Voltmeter::do_save()
{
    QSaveFile file(mFileName);
    qDebug("save");

    if (!file.open(QIODevice::WriteOnly)) {
        throw std::exception();
    }

    QTextStream stream(&file);

    stream << QString("Lenlab red %1.%2 Voltmeter-Daten\n").arg(MAJOR).arg(MINOR);

    stream << "Zeit";
    /*
    for (size_t i = 1; i < data.size(); i++) {
        if (m_channels[i-1]) {
            stream << DELIMITER << "Kanal_" << i;
        }
    }
    */
    stream << "\n";

    /*
    for (int t = 0; t < data[0].size(); t++) {
        stream << data[0][t];
        for (size_t i = 1; i < data.size(); i++) {
            if (m_channels[i-1]) {
                stream << DELIMITER << data[i][t];
            }
        }
        stream << "\n";
    }
    */

    file.commit();
}

void
Voltmeter::on_logger_data(protocol::pMessage const & reply)
{
    Q_ASSERT(reply->getUInt32BufferLength() == m_loggerseries->getChannels());
    uint32_t *buffer = reply->getUInt32Buffer();

    //qDebug("Voltmeter::on_logger");

    // TODO Implement time stamp???

    for (std::size_t i = 0; i < reply->getUInt32BufferLength(); ++i)
        m_loggerseries->append(i, static_cast< double >(buffer[i]) / VOLT);

    /*
    for (int i = 0; i < 2; i++)
        data[i].append(buffer[i] / VOLT);
    */

    //uint32_t *buffer = (uint32_t *) reply->getBody();

    //Q_ASSERT(reply->getBodyLength() == 4 * data.size());

    // 4 bytes timestamp
    //data[0].append(time_offset + (double) *buffer / MSEC);

    // 4 channels, 4 bytes each
    // old comment!
    //for (size_t i = 1; i < data.size(); i++) {
    //    data[i].append((double) buffer[i] / (double) reply->getBody()[0] / VOLT); // TODO Fix for new communictaion
    //}

    setMeasurementData(true);
    setUnsavedData(true);

    emit seriesUpdated();

}

void
Voltmeter::on_autosave()
{
    if (mUnsavedData) {
        try {
            do_save();
            setUnsavedData(false);
        }
        catch (std::exception) {
            setAutoSave(false);
            qDebug("Voltmeter: Fehler beim automatischen Speichern."); // TODO error signal and include reason
        }
    }
}

void
Voltmeter::on_start(protocol::pTask const & task)
{
    Q_UNUSED(task);
}

void
Voltmeter::on_start_failed(protocol::pTask const & task)
{
    Q_UNUSED(task);

    Q_ASSERT(false); // TODO error signal and disconnect board
}

void Voltmeter::on_stop(protocol::pTask const & task)
{
    Q_UNUSED(task);

    super::stop();
}

void
Voltmeter::on_stop_failed(protocol::pTask const & task)
{
    Q_UNUSED(task);

    Q_ASSERT(false); // TODO error signal and disconnect board
}

} // namespace model
