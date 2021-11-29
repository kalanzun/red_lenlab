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
    Q_PROPERTY(int trigger READ trigger WRITE setTrigger) // offset
    Q_PROPERTY(int view READ view WRITE setView) // length
    Q_PROPERTY(double yrange READ yrange WRITE setYRange) // value

    static const int m_channels = 2;
    // 18 packets a 504 samples
    static const int m_packet_length = 504;
    static const int m_length = 18 * m_packet_length;
    static const int m_max_view = 16 * m_packet_length; // das letzte Paket wird nie verwendet, 17 Pakete würden ausreichen

    std::array< int, m_channels > index;
    std::array< std::array< double, m_length >, m_channels > data;

    double m_samplerate = 250e3;
    int m_trigger = 0;
    int m_view = 8000;
    double m_yrange = 4;

public:
    explicit Waveform();

    void setSamplerate(double samplerate);
    double samplerate() const;

    void setTrigger(int trigger);
    int trigger() const;

    void setView(int view);
    int view() const;

    void setYRange(double yrange);
    double yrange() const;

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
