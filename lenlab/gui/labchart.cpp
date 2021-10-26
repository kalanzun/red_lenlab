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

#include "labchart.h"
#include "ui_labchart.h"

#include <QtCharts>
#include <QtPrintSupport/QtPrintSupport>

namespace gui {

LabChart::LabChart(QWidget *parent) :
    QWidget(parent),
    m_chart(new QChart),
    ui(new Ui::LabChart)
{
    ui->setupUi(this);

    m_chart->legend()->hide();
    ui->chartView->setChart(m_chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

    ui->label_y2->hide();
}

LabChart::~LabChart()
{
    delete ui;
}

QPointer< QChart >
LabChart::chart() const
{
    return m_chart;
}

QList< QPointer< QLineSeries > >
LabChart::series() const
{
    return m_series;
}

void
LabChart::setLabelX(QString text)
{
    ui->label_x->setText(text);
}

void
LabChart::setLabelY(QString text)
{
    ui->label_y->setText(text);
}

void
LabChart::setLabelY2(QString text)
{
    ui->label_y2->setText(text);
    ui->label_y2->show();
}

void
LabChart::addSeries(QLineSeries *series)
{
    m_chart->addSeries(series);
    m_series.append(series);
}

void
LabChart::addAxis(QAbstractAxis *axis, Qt::Alignment alignment)
{
    m_chart->addAxis(axis, alignment);
}

void
LabChart::createDefaultAxes()
{
    m_chart->createDefaultAxes();
    qobject_cast< QValueAxis * >(m_chart->axes(Qt::Horizontal).at(0))->setLabelFormat("%g");
}

void
LabChart::replace(model::pSeries const & series)
{
    QList< QPointF > points(series->getLength());

    for (unsigned int channel = 0; channel < m_series.size(); ++channel) {
        for (unsigned int i = 0; i < series->getLength(); ++i) {
            points[i] = QPointF(series->getX(i), series->getY(i, channel));
        }

        m_series.at(channel)->replace(points);
    }

    setRange(series);
}

void
LabChart::appendLast(model::pSeries const & series)
{
    for (unsigned int channel = 0; channel < m_series.size(); ++channel) {
        m_series.at(channel)->append(series->getLastX(), series->getLastY(channel));
    }

    setRange(series);
}

void
LabChart::setRange(model::pSeries const & series)
{
    auto axes = m_chart->axes(Qt::Horizontal);
    if (!axes.isEmpty()) axes.first()->setRange(series->getMinX(), series->getMaxX());

    axes = m_chart->axes(Qt::Vertical);
    if (!axes.isEmpty()) axes.first()->setRange(series->getMinY(0), series->getMaxY(0));
    if (axes.size() > 1) axes.at(1)->setRange(series->getMinY(1), series->getMaxY(1));
}

void
LabChart::setChannelVisible(unsigned int channel, bool visible)
{
    Q_ASSERT(channel < m_series.size());

    m_series.at(channel)->setVisible(visible);
}

void
LabChart::print(QString filename)
{
    // TODO switch to QPdfWriter

    QPrinter printer = QPrinter(QPrinter::HighResolution);
    printer.setOutputFileName(filename);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize::A5);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(), QPageLayout::Point);

    // note: it does overwrite without question

    QPainter painter;
    painter.begin(&printer);

    auto pageRect = printer.pageRect(QPrinter::Point);
    resize(pageRect.width(), pageRect.height());

    qreal scale = printer.resolution() / 72.0;
    painter.scale(scale, scale);

    render(&painter);

    painter.end();
}

} // namespace gui
