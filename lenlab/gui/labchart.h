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

#ifndef GUI_LABCHART_H
#define GUI_LABCHART_H

#include "model/lenlab.h"

#include <QWidget>
#include <QtCharts>

namespace gui {

namespace Ui {
class LabChart;
}

class LabChart : public QWidget
{
    Q_OBJECT

    QPointer< QChart > m_chart;
    QList< QPointer< QLineSeries > > m_series;

public:
    explicit LabChart(QWidget *parent = nullptr);
    ~LabChart();

    QPointer< QChart > chart() const;
    QList< QPointer< QLineSeries > > series() const;

    void setLabelX(QString text);
    void setLabelY(QString text);
    void setLabelY2(QString text);

    void addSeries(QLineSeries *series);
    void addAxis(QAbstractAxis *axis, Qt::Alignment alignment);
    void createDefaultAxes();

    void replace(model::pSeries const & series);
    void appendLast(model::pSeries const & series);
    void setRange(model::pSeries const & series);

    void setChannelVisible(unsigned int channel, bool visible);

    void print(QString filename);

private:
    Ui::LabChart *ui;
};


} // namespace gui
#endif // GUI_LABCHART_H
