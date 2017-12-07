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

#include "oscilloscopeform.h"
#include "ui_oscilloscopeform.h"
#include "pointvectorseriesdata.h"
#include "qwt_text.h"
#include "qwt_plot_renderer.h"
#include <QDebug>

namespace gui {

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeForm)
{
    qDebug() << "OscilloscopeForm";
    ui->setupUi(this);
}

OscilloscopeForm::~OscilloscopeForm()
{
    qDebug() << "~OscilloscopeForm";
    delete ui;
}

void
OscilloscopeForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

void
OscilloscopeForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->oscilloscope = lenlab->oscilloscope;

    curves[0] = newCurve(&oscilloscope->data[0], &oscilloscope->data[1], QColor("#fce94f"), true); // butter 0
    curves[1] = newCurve(&oscilloscope->data[0], &oscilloscope->data[2], QColor("#8ae234"), false); // green 0
    curves[2] = newCurve(&oscilloscope->data[0], &oscilloscope->data[3], QColor("#729fcf"), false); // sky blue 0
    curves[3] = newCurve(&oscilloscope->data[0], &oscilloscope->data[4], QColor("#ef2929"), false); // scarlet red 0

    connect(oscilloscope, SIGNAL(replot()),
            this, SLOT(on_replot()));
}

QwtPlotCurve *
OscilloscopeForm::newCurve(model::MinMaxVector *time, model::MinMaxVector *value, const QColor &color, bool visible)
{
    std::unique_ptr<QwtPlotCurve> curve(new QwtPlotCurve());

    curve->setSamples(new PointVectorSeriesData(time, value)); // acquires ownership
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->setVisible(visible);

    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    curve->setPen(pen);

    curve->attach(ui->plot); // acquires ownership
    return curve.release();
}

void
OscilloscopeForm::on_startButton_clicked()
{
    if (!oscilloscope->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(oscilloscope)) return;
        }
        oscilloscope->start();
    }
}

void
OscilloscopeForm::on_stopButton_clicked()
{
    if (oscilloscope->active()) {
        oscilloscope->stop();
    }
}

void
OscilloscopeForm::on_replot()
{
    ui->plot->replot();
}


} // namespace gui
