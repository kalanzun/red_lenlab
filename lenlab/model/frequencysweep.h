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
#include <QObject>

namespace model {

/**
 * @brief Lenlab Frequency Analysis component.
 */
class Frequencysweep : public Component
{
    Q_OBJECT

    uint8_t index, divider;

    QSharedPointer<FrequencySeries> current;

    bool running;
    bool toggle;

public:
    explicit Frequencysweep(Lenlab *parent);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    virtual void start();
    virtual void stop();

    virtual void timerEvent(QTimerEvent *event);

    void restart();

    void receive(const usb::pMessage &reply);

    QSharedPointer<FrequencySeries> getWaveform();

    void save(const QString &fileName);

signals:
    void replot();

public slots:
    void on_replot();

private:
    typedef Component super;
};

} // namespace model

#endif // FREQUENCYSWEEP_H
