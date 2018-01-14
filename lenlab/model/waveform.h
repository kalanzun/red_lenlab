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

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QObject>
#include <QVector>

namespace model {

// TODO: 2 Klassen: IncrementalWaveform und SingleshotWaveform
// std::array ist für das Oszi viel schneller als QVector

class Waveform : public QObject
{
    Q_OBJECT

    Q_PROPERTY(uint32_t samplerate READ samplerate WRITE setSamplerate)
    Q_PROPERTY(uint32_t trigger READ trigger WRITE setTrigger)
    Q_PROPERTY(uint32_t view READ view WRITE setView)

    uint32_t m_samplerate = 0;
    uint32_t m_trigger = 0;
    uint32_t m_view = 0;

    QVector< QVector< double >> data;

public:
    explicit Waveform(uint32_t channels, QObject *parent = nullptr);

    void setSamplerate(uint32_t samplerate);
    uint32_t samplerate();

    void setTrigger(uint32_t trigger);
    uint32_t trigger();

    void setView(uint32_t view);
    uint32_t view();

    void append(uint32_t channel, double value);

    uint32_t getLength();

    double getTime(uint32_t i);
    double getValue(uint32_t channel, uint32_t i);

    double getMinTime();
    double getMaxTime();
    double getMinValue();
    double getMaxValue();

signals:

public slots:
};

} // namespace model

#endif // WAVEFORM_H
