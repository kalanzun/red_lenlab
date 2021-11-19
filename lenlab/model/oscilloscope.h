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

#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "component.h"
#include "waveform.h"
#include "indexparameter.h"

#include <QObject>
#include <QTextStream>
#include <QTimer>

namespace model {

class Oscilloscope : public Component
{
    Q_OBJECT

    typedef Component super;

    static char const * const DELIMITER;

    static int const m_task_delay = 100;
    static int const m_task_timeout = 400;

    Q_PROPERTY(int samplerate_index READ samplerateIndex WRITE setSamplerateIndex)
    Q_PROPERTY(int view_index READ viewIndex WRITE setViewIndex)

    bool pending = 0;

    int m_samplerate_index = 0;
    int m_view_index = 0;
    QSharedPointer<Waveform> incoming;
    QSharedPointer<Waveform> waveform;

    QTimer startTimer;

public:
    static int const samplerate_count = 3;
    static int const view_count = 4;

    explicit Oscilloscope(Lenlab &lenlab, protocol::Board &board);
    Oscilloscope(Oscilloscope const &) = delete;

    Oscilloscope & operator=(Oscilloscope const &) = delete;

    virtual QString const & getNameNominative() const;
    virtual QString const & getNameAccusative() const;

    virtual pSeries getSeries() const;

    virtual void start();
    virtual void stop();
    virtual void reset();

    void setSamplerateIndex(int index);
    int samplerateIndex() const;

    void setViewIndex(int index);
    int viewIndex() const;

    static QString getSamplerateLabel(int index);
    QString getViewLabel(int index) const;

    void save(QTextStream &stream);

private:
    static double to_double(uint16_t state);

    static int to_log2oversamples(int index);
    static double to_samplerate(int index);

    static int to_view(int index);

private slots:
    void on_start();
    void on_succeeded(protocol::pTask const &);
    void on_failed(protocol::pTask const &);
};

} // namespace model

#endif // OSCILLOSCOPE_H
