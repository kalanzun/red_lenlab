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
#include "lenlab.h"
#include "config.h"
#include <QSaveFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>

#define MSEC (1000.0)
#define VOLT (4096.0 / 3.3)

namespace model {

Voltmeter::Voltmeter(Lenlab *parent) : Component(parent)
{

}

QString
Voltmeter::getNameNominative()
{
    return "as Spannungsmessgerät";
}

QString
Voltmeter::getNameAccusative()
{
    return "as Spannungsmessgerät";
}

void
Voltmeter::setBoard(const QPointer<protocol::Board> &board)
{
    super::setBoard(board);

    connect(board.data(), &protocol::Board::logger,
            this, &Voltmeter::on_logger);
}

uint32_t
Voltmeter::getInterval() const
{
    return interval;
}

void
Voltmeter::setInterval(uint32_t interval)
{
    this->interval = interval;
}

void
Voltmeter::start()
{
    super::start();

    restart();
}

void
Voltmeter::restart()
{
    /* old comment!
    if (data[0].isEmpty())
        time_offset = 0;
    else
        time_offset = data[0].last() + interval / MSEC;
    */

    board->startLogger(interval);

    /*
    qDebug("set intervall");
    usb::pMessage cmdSetInterval(new usb::Message());
    //cmdSetInterval->setCommand(setLoggerInterval);
    *((uint32_t *) cmdSetInterval->getBody()) = interval;
    cmdSetInterval->setBodyLength(4);
    //lenlab->send(cmdSetInterval);

    qDebug("start");
    usb::pMessage cmdStart(new usb::Message());
    //cmdStart->setCommand(startLogger);
    //lenlab->send(cmdStart);
    */
}

void
Voltmeter::stop()
{
    //qDebug("stop");
    //usb::pMessage cmd(new usb::Message());
    //cmd->setCommand(stopLogger);
    //lenlab->send(cmd);

    board->stopLogger();

    super::stop();
}

void
Voltmeter::clear()
{
    if (m_active)
        stop();

    //for (auto &vector : data) vector.clear();

    setAutoSave(false);
    setFileName(QString());
    setUnsavedData(false);
    setMeasurementData(false);

    emit replot();
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
Voltmeter::setMeasurementData(bool measurementData)
{
    if (m_measurementData != measurementData)
    {
        m_measurementData = measurementData;
        emit measurementDataChanged(m_measurementData);
    }
}

bool
Voltmeter::measurementData() const
{
    return m_measurementData;
}

void
Voltmeter::setUnsavedData(bool unsavedData)
{
    if (m_unsavedData != unsavedData)
    {
        m_unsavedData = unsavedData;
        emit unsavedDataChanged(m_unsavedData);
    }
}

bool
Voltmeter::unsavedData() const
{
    return m_unsavedData;
}

void
Voltmeter::setAutoSave(bool autoSave)
{
    if (m_autoSave != autoSave) {
        m_autoSave = autoSave;
        if (m_autoSave) {
            Q_ASSERT_X(!m_fileName.isEmpty(), "Voltmeter::setAutoSave()", "No fileName set");
            timer_id = startTimer(3000);
        }
        else {
            killTimer(timer_id);
        }
        emit autoSaveChanged(m_autoSave);
    }
}

bool
Voltmeter::autoSave() const
{
    return m_autoSave;
}

void
Voltmeter::setFileName(const QString &fileName)
{
    if (m_fileName != fileName) {
        m_fileName = fileName;
        emit fileNameChanged(m_fileName);
    }
}

const QString &
Voltmeter::fileName() const
{
    return m_fileName;
}

void
Voltmeter::setChannels(const std::bitset<4> &channels)
{
    if (m_channels != channels) {
        m_channels = channels;
        emit channelsChanged(m_channels);
    }
}

const std::bitset<4> &
Voltmeter::channels() const
{
    return m_channels;
}

void
Voltmeter::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if (m_autoSave) {
        if (m_unsavedData) {
            try {
                _save();
                setUnsavedData(false);
            }
            catch (std::exception) {
                setAutoSave(false);
                emit lenlab->logMessage("Voltmeter: Fehler beim automatischen Speichern."); // TODO include reason
            }
        }
    }
    else {
        qDebug("Voltmeter timer event misfired.");
    }
}

void
Voltmeter::save(const QString &fileName)
{
    setAutoSave(false);
    setFileName(fileName);
    _save();
}

void Voltmeter::on_logger(const protocol::pMessage &reply)
{

    qDebug("receive");

    uint32_t *buffer = reply->getIntBuffer();
    Q_ASSERT(reply->getIntBufferLength() == 2);

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

    emit replot();

}

void
Voltmeter::_save()
{
    QSaveFile file(m_fileName);
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

} // namespace model
