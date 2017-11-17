#include "logger.h"
#include "lenlab.h"
#include <QSaveFile>
#include <QIODevice>
#include <QTextStream>

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

    super::start();
}

void
Logger::clear()
{
    this->fileName = QString();
    setAutoSave(false);
    for (auto vector : data) vector.clear();
}

void
Logger::receive(const usb::pMessage &reply)
{
    uint32_t *buffer = (uint32_t *) reply->getPayload();

    Q_ASSERT(reply->getPayloadLength() == 4 * data.size());

    // 4 bytes timestamp
    data[0].append((double) *buffer / MSEC);

    // 4 channels, 4 bytes each
    for (size_t i = 1; i < data.size(); i++) {
        data[i].append((double) buffer[i] / (double) reply->getHeader() / VOLT);
    }

    newData = true;

    emit lenlab->replot();
}

void
Logger::setAutoSave(bool autoSave)
{
    if (m_autoSave != autoSave) {
        m_autoSave = autoSave;
        if (m_autoSave) {
            timer_id = startTimer(1000);
        }
        else {
            killTimer(timer_id);
        }
        emit autoSaveChanged(autoSave);
    }
}

bool
Logger::autoSave() const
{
    return m_autoSave;
}

void
Logger::timerEvent(QTimerEvent *event)
{
    if (m_autoSave) {
        if (newData) {
            try {
                _save();
                throw std::exception();
            }
            catch (std::exception) {
                setAutoSave(false);
                emit lenlab->logMessage("Logger automatic save failed."); // TODO include reason
            }
        }
    }
    else {
        qDebug("Logger timer event misfired.");
    }
}

void
Logger::save(QString fileName)
{
    setAutoSave(false);
    this->fileName = fileName;
    _save();
}

void
Logger::_save()
{
    QSaveFile file(fileName);
    qDebug("save");

    if (!file.open(QIODevice::WriteOnly)) {
        throw std::exception();
    }

    QTextStream stream(&file);

    stream << "Lenlab Red Version 1.0 Save File\n";

    stream << "Zeit";
    for (size_t i = 1; i < data.size(); i++) {
        stream << ", " << "Kanal_" << i;
    }
    stream << "\n";

    for (size_t t = 0; t < data[0].size(); t++) {
        stream << data[0][t];
        for (size_t i = 1; i < data.size(); i++) {
            stream << ", " << data[i][t]; // TODO double format for save file
        }
        stream << "\n";
    }

    file.commit();

    newData = false;
}

} // namespace model
