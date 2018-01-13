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

#ifndef VOLTMETER_H
#define VOLTMETER_H

#include "component.h"
#include <QObject>
#include <bitset>

namespace model {

/**
 * @brief Lenlab Voltmeter component.
 */

class Voltmeter : public Component
{
    Q_OBJECT
    Q_PROPERTY(bool measurementData READ measurementData WRITE setMeasurementData NOTIFY measurementDataChanged)
    Q_PROPERTY(bool unsavedData READ unsavedData WRITE setUnsavedData NOTIFY unsavedDataChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(std::bitset<4> channels READ channels WRITE setChannels NOTIFY channelsChanged)

public:
    explicit Voltmeter(Lenlab *parent);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    uint32_t getInterval() const;
    void setInterval(uint32_t interval);

    virtual void start();
    virtual void stop();
    void clear();

    virtual void receive(const usb::pMessage &reply);
    virtual void ready();

    void restart();

    void setMeasurementData(bool measurementData);
    bool measurementData() const;

    void setUnsavedData(bool unsavedData);
    bool unsavedData() const;

    void setAutoSave(bool autoSave);
    bool autoSave() const;

    void setFileName(const QString &fileName);
    const QString &fileName() const;

    void setChannels(const std::bitset<4> &channels);
    const std::bitset<4> &channels() const;

    void save(const QString &fileName);

    //std::array<MinMaxVector, 5> data;

signals:
    void replot();
    void measurementDataChanged(bool);
    void unsavedDataChanged(bool);
    void autoSaveChanged(bool);
    void fileNameChanged(const QString &);
    void channelsChanged(const std::bitset<4> &);

public slots:

private:
    typedef Component super;

    void _save();
    virtual void timerEvent(QTimerEvent *event);

    uint32_t interval = 1000;

    bool m_measurementData = false;
    bool m_unsavedData = false;
    bool m_autoSave = false;
    QString m_fileName;
    std::bitset<4> m_channels = 1;

    int timer_id = 0;
    double time_offset;

};

} // namespace model

#endif // VOLTMETER_H
