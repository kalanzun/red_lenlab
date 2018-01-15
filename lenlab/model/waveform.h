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

#include "series.h"
#include <QObject>
#include <QVector>

namespace model {

class Waveform : public Series
{
    Q_OBJECT

    Q_PROPERTY(uint32_t samplerate READ samplerate WRITE setSamplerate)
    Q_PROPERTY(uint32_t trigger READ trigger WRITE setTrigger)
    Q_PROPERTY(uint32_t view READ view WRITE setView)

    uint32_t m_samplerate = 0;
    uint32_t m_trigger = 0;
    uint32_t m_view = 0;

public:
    explicit Waveform();

    void setSamplerate(uint32_t samplerate);
    uint32_t samplerate();

    void setTrigger(uint32_t trigger);
    uint32_t trigger();

    void setView(uint32_t view);
    uint32_t view();

    void append(uint32_t channel, double value);

    uint32_t getLength(uint32_t channel);

    double getX(uint32_t i);
    double getY(uint32_t i, uint32_t channel);

    double getMinX();
    double getMaxX();
    double getMinY(uint32_t channel);
    double getMaxY(uint32_t channel);

signals:

public slots:

private:
    typedef Series super;

    std::array< uint32_t, 2 > index{{0, 0}};
    std::array< std::array< double, 7000 >, 2 > data;

};

} // namespace model

#endif // WAVEFORM_H
