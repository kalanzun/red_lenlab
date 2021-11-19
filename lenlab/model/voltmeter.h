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

#ifndef VOLTMETER_H
#define VOLTMETER_H

#include "component.h"
#include "loggerseries.h"

#include <QObject>
#include <QSharedPointer>
#include <QTextStream>
#include <QTimer>

#include <bitset>

namespace model {

class Voltmeter : public Component
{
    Q_OBJECT

    typedef Component super;

    static char const * const DELIMITER;

    Q_PROPERTY(bool measurementData READ measurementData WRITE setMeasurementData NOTIFY measurementDataChanged)
    Q_PROPERTY(bool unsavedData READ unsavedData WRITE setUnsavedData NOTIFY unsavedDataChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(std::bitset<4> channels READ channels WRITE setChannels NOTIFY channelsChanged)
    Q_PROPERTY(int interval_index READ getIntervalIndex WRITE setIntervalIndex NOTIFY intervalChanged)

    static double const MSEC;
    static double const VOLT;

    static std::array< int const, 6 > const interval;

    bool mMeasurementData = false;
    bool mUnsavedData = false;
    bool mAutoSave = false;
    QString mFileName;
    std::bitset<4> mChannels = 1;
    int m_interval_index = 3;

    QSharedPointer<Loggerseries> m_loggerseries;

    QTimer mAutoSaveTimer;
    double mOffsetTime;

public:
    static int const interval_count = interval.size();

    explicit Voltmeter(Lenlab & lenlab, protocol::Board & board);
    Voltmeter(Voltmeter const &) = delete;

    Voltmeter & operator=(Voltmeter const &) = delete;

    virtual QString const & getNameNominative() const;
    virtual QString const & getNameAccusative() const;

    static QString getIntervalLabel(int index);

    void setMeasurementData(bool measurementData);
    bool measurementData() const;

    void setUnsavedData(bool unsavedData);
    bool unsavedData() const;

    void setAutoSave(bool autoSave);
    bool autoSave() const;

    void setFileName(QString const & fileName);
    const QString &fileName() const;

    void setChannels(std::bitset<4> const & channels);
    const std::bitset<4> &channels() const;

    void setIntervalIndex(int index);
    int getIntervalIndex() const;

    virtual pSeries getSeries() const;

    virtual void start();
    virtual void stop();
    virtual void reset();

    void clear();

    //virtual void receive(const usb::pMessage &reply);
    //virtual void ready();

    void save(QTextStream &stream);

private:
    static int to_interval(int index);

signals:
    void measurementDataChanged(bool);
    void unsavedDataChanged(bool);
    void autoSaveChanged(bool);
    void fileNameChanged(QString const &);
    void channelsChanged(std::bitset<4> const &);
    void intervalChanged(uint32_t);

private slots:
    void on_autosave();

    void on_start(protocol::pTask const &);
    void on_start_failed(protocol::pTask const &);
    void on_stop(protocol::pTask const &);
    void on_stop_failed(protocol::pTask const &);

    void on_logger_data(protocol::pMessage const &);
    void on_error_message(protocol::pMessage const &);
};

} // namespace model

#endif // VOLTMETER_H
