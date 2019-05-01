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

#include "frequencyform.h"
#include "ui_frequencyform.h"
#include "pointvectorseriesdata.h"
#include "qwt_text.h"
#include "qwt_plot_renderer.h"
#include "qwt_scale_engine.h"
//#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

namespace gui {

FrequencyForm::FrequencyForm(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::FrequencyForm)
{
    ui->setupUi(this);

    ui->plot->enableAxis(QwtPlot::yRight);

    QwtLogScaleEngine* xScaleEngine = new QwtLogScaleEngine();
    ui->plot->setAxisScaleEngine(QwtPlot::xBottom, xScaleEngine);
    ui->plot->setAxisMaxMinor(QwtPlot::xBottom, 10);

    QwtText x_label("Frequenz [Hz]");
    QFont x_font(ui->plot->axisFont(2));
    x_label.setFont(x_font);
    ui->plot->setAxisTitle(2, x_label);

    QwtText y_label("Amplitude [dB]");
    QFont y_font(ui->plot->axisFont(0));
    y_label.setFont(y_font);
    ui->plot->setAxisTitle(0, y_label);

    QwtText phase_label("Phase [°]");
    QFont phase_font(ui->plot->axisFont(QwtPlot::yRight));
    phase_label.setFont(phase_font);
    ui->plot->setAxisTitle(QwtPlot::yRight, phase_label);

    //QwtLogScaleEngine* yAmplitudeScaleEngine = new QwtLogScaleEngine();
    //ui->plot->setAxisScaleEngine(QwtPlot::yLeft, yAmplitudeScaleEngine);

    m_curves[0] = newCurve(QColor("#729fcf"), true); // sky blue 0
    m_curves[1] = newCurve(QColor("#ef2929"), true); // scarlet red 0
    m_curves[1]->setYAxis(QwtPlot::yRight);

    newGrid();
}

FrequencyForm::~FrequencyForm()
{
    delete ui;
}

void
FrequencyForm::setMainWindow(MainWindow *main_window)
{
    m_main_window = main_window;
}

void
FrequencyForm::setModel(model::Lenlab *lenlab)
{
    m_lenlab = lenlab;
    m_frequencysweep = &lenlab->frequencysweep;

    connect(m_frequencysweep, &model::Frequencysweep::seriesChanged,
            this, &FrequencyForm::seriesChanged);
    connect(m_frequencysweep, &model::Frequencysweep::seriesUpdated,
            this, &FrequencyForm::seriesUpdated);
}

QwtPlotCurve *
FrequencyForm::newCurve(const QColor &color, bool visible)
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
FrequencyForm::newGrid()
{
    std::unique_ptr<QwtPlotGrid> grid(new QwtPlotGrid());

    QPen pen;
    pen.setStyle(Qt::DotLine);
    pen.setColor("#555753"); // aluminium 4
    grid->setPen(pen);
    grid->enableXMin(true);

    grid->attach(ui->plot); // acquires ownership
    return grid.release();
}

void
FrequencyForm::on_startButton_clicked()
{
    if (!m_frequencysweep->active()) {
        if (m_lenlab->isActive()) {
            if (!m_main_window->askToCancelActiveComponent(m_frequencysweep)) return;
        }
        m_frequencysweep->start();
    }
}

void
FrequencyForm::on_stopButton_clicked()
{
    if (m_frequencysweep->active()) {
        m_frequencysweep->stop();
    }
}

void
FrequencyForm::on_saveButton_clicked()
{
    save();
}

void
FrequencyForm::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern");
    try {
        m_frequencysweep->save(fileName);
    }
    catch (std::exception const &) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
FrequencyForm::saveImage()
{
    QwtPlotRenderer renderer;
    renderer.exportTo(ui->plot, "bode.pdf"); // it asks for the filename
}

void
FrequencyForm::seriesUpdated()
{
    ui->plot->replot();
}

void
FrequencyForm::seriesChanged(model::pSeries const & series)
{
    m_curves[0]->setSamples(new PointVectorSeriesData(series, 1)); // acquires ownership
    m_curves[1]->setSamples(new PointVectorSeriesData(series, 2)); // acquires ownership
}

} // namespace gui
