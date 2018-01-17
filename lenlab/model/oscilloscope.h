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

#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "component.h"
#include "communication.h"
#include "waveform.h"
#include "indexparameter.h"
#include <QObject>

namespace model {

/**
 * @brief Lenlab oscilloscope component.
 */

class Oscilloscope : public Component
{
    Q_OBJECT
public:
    explicit Oscilloscope(Lenlab *parent);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    virtual void start();
    virtual void stop();
    void single();

    void try_to_start();
    void restart();

    QSharedPointer<Waveform> getWaveform();

    void setSamplerate(uint32_t index);

    IndexParameter samplerateIndex;

signals:
    void replot();

public slots:
    void on_reply(const pCommunication &com, const usb::pMessage &reply);

private:
    bool pending = 0;

    QSharedPointer<Waveform> incoming;
    QSharedPointer<Waveform> current;

    uint32_t samplerate = 0;

    typedef Component super;
};

} // namespace model

#endif // OSCILLOSCOPE_H
