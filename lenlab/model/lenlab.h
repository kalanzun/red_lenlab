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

#ifndef LENLAB_H
#define LENLAB_H

#include "frequencysweep.h"
#include "voltmeter.h"
#include "oscilloscope.h"
#include "signalgenerator.h"
#include "protocol/factory.h"
#include "protocol/message.h"
#include <QObject>
#include <QPointer>

namespace model {

/**
 * @brief Root object of the Lenlab data model.
 */

class Lenlab : public QObject
{
    Q_OBJECT
public:
    explicit Lenlab(QObject *parent = nullptr);
    virtual ~Lenlab();

    bool isActive();
    Component *getActiveComponent();

    bool available();

    Frequencysweep *frequencysweep;
    Voltmeter *voltmeter;
    Oscilloscope *oscilloscope;
    Signalgenerator *signalgenerator;

signals:
    void receive(const protocol::pMessage &);

    void logMessage(const QString &);

    void replot();

public slots:

private slots:
    void on_error(const QString &);
    void on_ready(const QPointer<protocol::Board> &);

};

} // namespace model

#endif // LENLAB_H
