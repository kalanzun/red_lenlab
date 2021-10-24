/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
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

#include "oscilloscopeform.h"
#include "ui_oscilloscopeform.h"
//#include "pointvectorseriesdata.h"
//#include "qwt_text.h"
//#include "qwt_plot_renderer.h"
//#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCharts>

namespace gui {

OscilloscopeForm::OscilloscopeForm(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::OscilloscopeForm)
{
    ui->setupUi(this);

    QChart *chart = new QChart();
    chart->legend()->hide();

    for (size_t i = 0; i < m_series.size(); ++i) {
        m_series[i] = new QLineSeries();
        chart->addSeries(m_series[i]);
    }

    chart->createDefaultAxes();

    ui->plot->setChart(chart);
    ui->plot->setRenderHint(QPainter::Antialiasing);

    /*
    QwtText x_label("Zeit [ms]");
    QFont x_font(ui->plot->axisFont(2));
    x_label.setFont(x_font);
    ui->plot->setAxisTitle(2, x_label);

    QwtText y_label("Spannung [V]");
    QFont y_font(ui->plot->axisFont(0));
    y_label.setFont(y_font);
    ui->plot->setAxisTitle(0, y_label);

    for (int i = 0; i < 7; ++i)
        ui->timerangeBox->insertItem(i, QString("%L1 ms").arg(0.5*(1<<i)));
    ui->timerangeBox->setCurrentIndex(3);

    m_curves[0] = newCurve(QColor("#729fcf"), true); // sky blue 0
    //m_curves[0] = newCurve(QColor("#8ae234"), true); // green 0
    m_curves[1] = newCurve(QColor("#ef2929"), true); // scarlet red 0
    //m_curves[1] = newCurve(QColor("#fce94f"), true); // butter 0

    newGrid();
    */
}

OscilloscopeForm::~OscilloscopeForm()
{
    delete ui;
}

void
OscilloscopeForm::setMainWindow(MainWindow * main_window)
{
    m_main_window = main_window;
}

void
OscilloscopeForm::setModel(model::Lenlab * lenlab)
{
    m_lenlab = lenlab;
    m_oscilloscope = &lenlab->oscilloscope;

    ui->samplerateBox->insertItems(0, m_oscilloscope->samplerateIndex.labels);

    connect(m_oscilloscope, &model::Oscilloscope::seriesChanged,
            this, &OscilloscopeForm::seriesChanged);

    connect(&m_lenlab->voltmeter, &model::Voltmeter::activeChanged,
            this, &OscilloscopeForm::activeChanged);
    connect(&m_lenlab->frequencysweep, &model::Oscilloscope::activeChanged,
            this, &OscilloscopeForm::activeChanged);
}

/*
QwtPlotCurve *
OscilloscopeForm::newCurve(const QColor &color, bool visible)
{
    std::unique_ptr<QwtPlotCurve> curve(new QwtPlotCurve());

    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setVisible(visible);

    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    curve->setPen(pen);

    curve->attach(ui->plot); // acquires ownership
    return curve.release();
}

QwtPlotGrid *
OscilloscopeForm::newGrid()
{
    std::unique_ptr<QwtPlotGrid> grid(new QwtPlotGrid());

    QPen pen;
    pen.setStyle(Qt::DotLine);
    pen.setColor("#555753"); // aluminium 4
    grid->setPen(pen);

    grid->attach(ui->plot); // acquires ownership
    return grid.release();
}
*/

void
OscilloscopeForm::on_startButton_clicked()
{
    if (!m_oscilloscope->active()) {
        if (m_lenlab->isActive()) {
            if (m_main_window->askToCancelActiveComponent(m_oscilloscope)) {
                if (m_lenlab->isActive()) {
                    // the component might have stopped while the dialog was visible
                    pending = true;
                    m_lenlab->getActiveComponent()->stop();
                } else {
                    m_oscilloscope->start();
                }
            }
        } else {
            m_oscilloscope->start();
        }
    }
}

void
OscilloscopeForm::on_stopButton_clicked()
{
    if (m_oscilloscope->active()) {
        m_oscilloscope->stop();
    }
}

void
OscilloscopeForm::seriesChanged(model::pSeries const & series)
{
    QList< QPointF > points(series->getLength());

    for (size_t channel = 0; channel < m_series.size(); ++channel) {
        for (size_t i = 0; i < series->getLength(); ++i)
            points[i] = QPointF(series->getX(i), series->getY(i, channel));

        m_series[channel]->replace(points);
    }

    ui->plot->chart()->axes(Qt::Horizontal)[0]->setRange(series->getMinX(), series->getMaxX());
    ui->plot->chart()->axes(Qt::Vertical)[0]->setRange(series->getMinY(0), series->getMaxY(0));
    /*
    for (unsigned int i = 0; i < m_curves.size(); ++i) {
        m_curves[i]->setSamples(new PointVectorSeriesData(series, i)); // acquires ownership
    }
    ui->plot->replot();
    */
}

void
OscilloscopeForm::on_samplerateBox_activated(int index)
{
    m_oscilloscope->setSamplerate(static_cast<uint32_t>(index));
}

void
OscilloscopeForm::on_ch1CheckBox_stateChanged(int state)
{
    //m_curves[0]->setVisible(state == Qt::Checked);
}

void
OscilloscopeForm::on_ch2CheckBox_stateChanged(int state)
{
    //m_curves[1]->setVisible(state == Qt::Checked);
}

void
OscilloscopeForm::on_saveButton_clicked()
{
    save();
}

void
OscilloscopeForm::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern", "oszilloskop.csv", tr("CSV (*.csv)"));
    try {
        m_oscilloscope->save(fileName);
    }
    catch (std::exception const &) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
OscilloscopeForm::saveImage()
{
    /*
    QwtPlotRenderer renderer;
    renderer.exportTo(ui->plot, "oszilloskop.pdf"); // it asks for the filename
    */
}

void
OscilloscopeForm::on_timerangeBox_currentIndexChanged(int index)
{
    /*
    double timerange = 0.5 * (1<<index);
    ui->plot->setAxisScale(QwtPlot::xBottom, -timerange/2, timerange/2);
    ui->plot->replot();
    */
}

void OscilloscopeForm::activeChanged(bool)
{
    if (pending) {
        pending = false;
        if (!m_lenlab->isActive()) m_oscilloscope->start();
    }
}

} // namespace gui
