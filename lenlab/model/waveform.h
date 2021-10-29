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

#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "series.h"

#include <QObject>

#include <array>

namespace model {

class Waveform : public Series
{
    Q_OBJECT

    typedef Series super;

    Q_PROPERTY(double samplerate READ samplerate WRITE setSamplerate)
    Q_PROPERTY(uint32_t trigger READ trigger WRITE setTrigger) // offset
    Q_PROPERTY(uint32_t view READ view WRITE setView) // length

    static const int m_channels = 2;
    // 18 packets a 504 samples
    static const int m_length = 18 * 504;

    std::array< int, m_channels > index;
    std::array< std::array< double, m_length >, m_channels > data;

    double m_samplerate = 250e3;
    unsigned int m_trigger = 0;
    unsigned int m_view = 16 * 504; // das letzte Paket wird nie verwendet, 17 Pakete würden ausreichen

public:
    explicit Waveform();

    void setSamplerate(double samplerate);
    double samplerate();

    void setTrigger(unsigned int trigger);
    unsigned int trigger();

    void setView(unsigned int view);
    unsigned int view();

    void append(int channel, double value);

    int getChannels() const;
    int getLength() const;

    double getX(int i) const;
    double getY(int i, int channel) const;

    double getMinX() const;
    double getMaxX() const;
    double getMinY(int channel) const;
    double getMaxY(int channel) const;
};

} // namespace model

#endif // WAVEFORM_H
