/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#ifndef FREQUENCYSWEEP_H
#define FREQUENCYSWEEP_H

#include "component.h"
#include "frequencyseries.h"
#include "signalgenerator.h"

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

namespace model {


class Frequencysweep : public Component
{
    Q_OBJECT

    typedef Component super;

    static char const * const DELIMITER;

    static int const m_channels = 2;
    static int const m_uint16_offset = 6;

    static int const m_task_delay = 10;
    static int const m_task_timeout = 200;

    Signalgenerator & m_signalgenerator;
    QSharedPointer<FrequencySeries> m_current;

    std::size_t m_index;
    uint32_t m_samplerate;
    int m_error_counter;
    int m_signalgenerator_error_counter;

    QTimer stepTimer;

public:
    typedef std::array< std::array < double, 5040 >, m_channels > OscilloscopeData;
    typedef QSharedPointer< OscilloscopeData > pOscilloscopeData;

    explicit Frequencysweep(Lenlab & lenlab, protocol::Board & board, Signalgenerator & signalgenerator);
    Frequencysweep(Frequencysweep const &) = delete;

    Frequencysweep & operator=(Frequencysweep const &) = delete;

    virtual QString const & getNameNominative() const;
    virtual QString const & getNameAccusative() const;

    virtual pSeries getSeries() const;

    virtual void start();
    virtual void stop();
    virtual void reset();

    void save(const QString &fileName);

signals:
    void calculate(pOscilloscopeData);

private slots:
    void on_signalgenerator_succeeded(protocol::pTask const &);
    void on_signalgenerator_failed(protocol::pTask const &);
    void on_step();
    void on_succeeded(protocol::pTask const &);
    void on_failed(protocol::pTask const &);
    void on_calculate(pOscilloscopeData);
};

} // namespace model

#endif // FREQUENCYSWEEP_H
