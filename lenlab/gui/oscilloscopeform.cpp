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
#include <QFileDialog>
#include <QMessageBox>

namespace gui {

OscilloscopeForm::OscilloscopeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeForm)
{
    qDebug() << "OscilloscopeForm";
    ui->setupUi(this);

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

    newGrid();
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
    /*
    this->lenlab = lenlab;
    this->oscilloscope = lenlab->oscilloscope;

    curves[0] = newCurve(QColor("#729fcf"), true); // sky blue 0
    curves[1] = newCurve(QColor("#8ae234"), true); // green 0
    //curves[0] = newCurve(QColor("#fce94f"), true); // butter 0
    //curves[3] = newCurve(QColor("#ef2929"), false); // scarlet red 0

    ui->samplerateBox->insertItems(0, oscilloscope->samplerateIndex.labels);

    connect(oscilloscope, SIGNAL(replot()),
            this, SLOT(on_replot()));
            */
}

QwtPlotCurve *
OscilloscopeForm::newCurve(const QColor &color, bool visible)
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
    /*
    for (unsigned int i = 0; i < curves.size(); ++i) {
        curves[i]->setSamples(new PointVectorSeriesData(oscilloscope->waveform, i)); // acquires ownership
    }
    ui->plot->replot();
    */
}

void
OscilloscopeForm::on_samplerateBox_activated(int index)
{
    oscilloscope->setSamplerate(static_cast<uint32_t>(index));
}

void
OscilloscopeForm::on_ch1CheckBox_stateChanged(int state)
{
    curves[0]->setVisible(state == Qt::Checked);
}

void
OscilloscopeForm::on_ch2CheckBox_stateChanged(int state)
{
    curves[1]->setVisible(state == Qt::Checked);
}

void
OscilloscopeForm::on_saveButton_clicked()
{
    save();
}

void
OscilloscopeForm::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern");
    try {
        oscilloscope->save(fileName);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
OscilloscopeForm::on_timerangeBox_currentIndexChanged(int index)
{
    double timerange = 0.5 * (1<<index);
    ui->plot->setAxisScale(QwtPlot::xBottom, -timerange/2, timerange/2);
    ui->plot->replot();
}

} // namespace gui
