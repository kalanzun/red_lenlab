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

#ifndef FREQUENCYSWEEP_H
#define FREQUENCYSWEEP_H

#include "component.h"
#include "frequencyseries.h"
#include "signalgenerator.h"
#include "waveform.h"

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

namespace model {

class Frequencysweep : public Component
{
    Q_OBJECT

    typedef Component super;

    static int const m_task_delay = 10;
    static int const m_task_timeout = 100;

    Signalgenerator & m_signalgenerator;
    QSharedPointer<FrequencySeries> m_current;

    uint8_t m_index;
    uint32_t m_samplerate;

    QSharedPointer<Waveform> m_incoming;

    QTimer stepTimer;

public:
    explicit Frequencysweep(Lenlab & lenlab, protocol::Board & board, Signalgenerator & signalgenerator);
    Frequencysweep(Frequencysweep const &) = delete;

    Frequencysweep & operator=(Frequencysweep const &) = delete;

    virtual QString const & getNameNominative() const;
    virtual QString const & getNameAccusative() const;

    virtual void start();
    virtual void stop();

    QSharedPointer<FrequencySeries> getWaveform();

    void save(const QString &fileName);

signals:
    void calculate();
    void replot();

private slots:
    void on_sine();
    void on_step();
    void on_succeeded(protocol::pTask const &);
    void on_failed(protocol::pTask const &);
    void on_calculate();
};

} // namespace model

#endif // FREQUENCYSWEEP_H
