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
#include <QDebug>

namespace gui {

FrequencyForm::FrequencyForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrequencyForm)
{
    qDebug() << "FrequencyForm";
    ui->setupUi(this);
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

    curves[0] = newCurve(QColor("#fce94f"), true); // butter 0
    curves[0]->setSamples(new PointVectorSeriesData(frequencysweep->getWaveform(), 0)); // acquires ownership
    //curves[1] = newCurve(QColor("#8ae234"), true); // green 0
    //curves[2] = newCurve(QColor("#729fcf"), false); // sky blue 0
    //curves[3] = newCurve(QColor("#ef2929"), false); // scarlet red 0

    connect(frequencysweep, SIGNAL(replot()),
            this, SLOT(on_replot()));
}

QwtPlotCurve *
FrequencyForm::newCurve(const QColor &color, bool visible)
{
    std::unique_ptr<QwtPlotCurve> curve(new QwtPlotCurve());

    //curve->setSamples(new PointVectorSeriesData(time, value)); // acquires ownership
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


} // namespace gui
