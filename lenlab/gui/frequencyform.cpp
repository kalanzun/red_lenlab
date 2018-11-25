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
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

namespace gui {

FrequencyForm::FrequencyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyForm)
{
    qDebug() << "FrequencyForm";
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
}

FrequencyForm::~FrequencyForm()
{
    qDebug() << "~FrequencyForm";
    delete ui;
}

void
FrequencyForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

void
FrequencyForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->frequencysweep = lenlab->frequencysweep;

    curves[0] = newCurve(1, QColor("#729fcf"), true); // sky blue 0
    curves[1] = newCurve(2, QColor("#ef2929"), true); // scarlet red 0
    curves[1]->setYAxis(QwtPlot::yRight);

    newGrid();

    connect(frequencysweep, SIGNAL(replot()),
            this, SLOT(on_replot()));
}

QwtPlotCurve *
FrequencyForm::newCurve(uint32_t channel, const QColor &color, bool visible)
{
    std::unique_ptr<QwtPlotCurve> curve(new QwtPlotCurve());

    curve->setSamples(new PointVectorSeriesData(frequencysweep->getWaveform(), channel)); // acquires ownership
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
    if (!frequencysweep->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(frequencysweep)) return;
        }
        frequencysweep->start();
    }
}

void
FrequencyForm::on_stopButton_clicked()
{
    if (frequencysweep->active()) {
        frequencysweep->stop();
    }
}

void
FrequencyForm::on_replot()
{
    ui->plot->replot();
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
        frequencysweep->save(fileName);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

} // namespace gui
