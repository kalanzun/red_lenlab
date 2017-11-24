#include "logger.h"
#include "lenlab.h"
#include "config.h"
#include <QSaveFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>

#define MSEC (1000.0)
#define VOLT (4096.0 / 3.3)

namespace model {

Logger::Logger(Lenlab *parent) : Component(parent)
{

}

QString
Logger::getNameNominative()
{
    return "er Logger";
}

QString
Logger::getNameAccusative()
{
    return "en Logger";
}

uint32_t
Logger::getInterval() const
{
    return interval;
}

void
Logger::setInterval(uint32_t interval)
{
    this->interval = interval;
}

void
Logger::start()
{
    super::start();

    restart();
}

void
Logger::restart()
{
    if (data[0].isEmpty())
        time_offset = 0;
    else
        time_offset = data[0].last() + interval / MSEC;

    qDebug("set intervall");
    usb::pMessage cmdSetInterval(new usb::Message());
    cmdSetInterval->setCommand(setLoggerInterval);
    *((uint32_t *) cmdSetInterval->getPayload()) = interval;
    cmdSetInterval->setPayloadLength(4);
    lenlab->send(cmdSetInterval);

    qDebug("start");
    usb::pMessage cmdStart(new usb::Message());
    cmdStart->setCommand(startLogger);
    lenlab->send(cmdStart);
}

void
Logger::stop()
{
    qDebug("stop");
    usb::pMessage cmd(new usb::Message());
    cmd->setCommand(stopLogger);
    lenlab->send(cmd);

    super::stop();
}

void
Logger::clear()
{
    if (m_active)
        stop();

    for (auto &vector : data) vector.clear();

    setAutoSave(false);
    setFileName(QString());
    setUnsavedData(false);
    setMeasurementData(false);

    emit replot();
}

void
Logger::receive(const usb::pMessage &reply)
{
    qDebug("receive");

    uint32_t *buffer = (uint32_t *) reply->getPayload();

    Q_ASSERT(reply->getPayloadLength() == 4 * data.size());

    // 4 bytes timestamp
    data[0].append(time_offset + (double) *buffer / MSEC);

    // 4 channels, 4 bytes each
    for (size_t i = 1; i < data.size(); i++) {
        data[i].append((double) buffer[i] / (double) reply->getHeader() / VOLT);
    }

    setMeasurementData(true);
    setUnsavedData(true);

    emit replot();
}

void
Logger::ready()
{
    if (m_active)
        restart();
}

void
Logger::setMeasurementData(bool measurementData)
{
    if (m_measurementData != measurementData)
    {
        m_measurementData = measurementData;
        emit measurementDataChanged(m_measurementData);
    }
}

bool
Logger::measurementData() const
{
    return m_measurementData;
}

void
Logger::setUnsavedData(bool unsavedData)
{
    if (m_unsavedData != unsavedData)
    {
        m_unsavedData = unsavedData;
        emit unsavedDataChanged(m_unsavedData);
    }
}

bool
Logger::unsavedData() const
{
    return m_unsavedData;
}

void
Logger::setAutoSave(bool autoSave)
{
    if (m_autoSave != autoSave) {
        m_autoSave = autoSave;
        if (m_autoSave) {
            Q_ASSERT_X(!m_fileName.isEmpty(), "Logger::setAutoSave()", "No fileName set");
            timer_id = startTimer(3000);
        }
        else {
            killTimer(timer_id);
        }
        emit autoSaveChanged(m_autoSave);
    }
}

bool
Logger::autoSave() const
{
    return m_autoSave;
}

void
Logger::setFileName(const QString &fileName)
{
    if (m_fileName != fileName) {
        m_fileName = fileName;
        emit fileNameChanged(m_fileName);
    }
}

const QString &
Logger::fileName() const
{
    return m_fileName;
}

void
Logger::setChannels(const std::bitset<4> &channels)
{
    if (m_channels != channels) {
        m_channels = channels;
        emit channelsChanged(m_channels);
    }
}

const std::bitset<4> &
Logger::channels() const
{
    return m_channels;
}

void
Logger::timerEvent(QTimerEvent *event)
{
    if (m_autoSave) {
        if (m_unsavedData) {
            try {
                _save();
                setUnsavedData(false);
                //throw std::exception();
            }
            catch (std::exception) {
                setAutoSave(false);
                emit lenlab->logMessage("Logger: Fehler beim automatischen Speichern."); // TODO include reason
            }
        }
    }
    else {
        qDebug("Logger timer event misfired.");
    }
}

void
Logger::save(const QString &fileName)
{
    setAutoSave(false);
    setFileName(fileName);
    _save();
}

void
Logger::_save()
{
    QSaveFile file(m_fileName);
    qDebug("save");

    if (!file.open(QIODevice::WriteOnly)) {
        throw std::exception();
    }

    QTextStream stream(&file);

    stream << QString("Lenlab red %1.%2 Logger-Daten\n").arg(MAJOR).arg(MINOR);

    stream << "Zeit";
    for (size_t i = 1; i < data.size(); i++) {
        if (m_channels[i-1]) {
            stream << DELIMITER << "Kanal_" << i;
        }
    }
    stream << "\n";

    for (int t = 0; t < data[0].size(); t++) {
        stream << data[0][t];
        for (size_t i = 1; i < data.size(); i++) {
            if (m_channels[i-1]) {
                stream << DELIMITER << data[i][t];
            }
        }
        stream << "\n";
    }

    file.commit();
}

} // namespace model
