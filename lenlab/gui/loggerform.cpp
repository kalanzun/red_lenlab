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

#include "loggerform.h"
#include "ui_loggerform.h"
#include "pointvectorseriesdata.h"
#include "mainwindow.h"
#include "qwt_text.h"
#include "qwt_plot_renderer.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QPen>
#include <QColor>

namespace gui {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    qDebug() << "LoggerForm";
    ui->setupUi(this);

    ui->autoSaveCheckBox->setEnabled(false);

    QwtText x_label("Zeit [s]");
    QFont x_font(ui->plot->axisFont(2));
    x_label.setFont(x_font);
    ui->plot->setAxisTitle(2, x_label);

    QwtText y_label("Spannung [V]");
    QFont y_font(ui->plot->axisFont(0));
    y_label.setFont(y_font);
    ui->plot->setAxisTitle(0, y_label);

    newGrid();
}

LoggerForm::~LoggerForm()
{
    qDebug() << "~LoggerForm";
    delete ui;
}

/**
 * @brief LoggerForm::setMainWindow
 * @param main_window
 *
 * It does not take ownership.
 */

void
LoggerForm::setMainWindow(MainWindow *main_window)
{
    this->main_window = main_window;
}

/**
 * @brief LoggerForm::setModel
 * @param lenlab
 *
 * It does not take ownership.
 */

void
LoggerForm::setModel(model::Lenlab *lenlab)
{
    this->lenlab = lenlab;
    this->voltmeter = lenlab->voltmeter;
/*
    curves[0] = newCurve(&voltmeter->data[0], &voltmeter->data[1], QColor("#edd400"), 2, true); // butter 1
    curves[1] = newCurve(&voltmeter->data[0], &voltmeter->data[2], QColor("#73d216"), 2, false); // green 1
    curves[2] = newCurve(&voltmeter->data[0], &voltmeter->data[3], QColor("#3465a4"), 2, false); // sky blue 1
    curves[3] = newCurve(&voltmeter->data[0], &voltmeter->data[4], QColor("#cc0000"), 2, false); // scarlet red 1
*/
    curves[0] = newCurve(&voltmeter->data[0], &voltmeter->data[1], QColor("#fce94f"), true); // butter 0
    curves[1] = newCurve(&voltmeter->data[0], &voltmeter->data[2], QColor("#8ae234"), false); // green 0
    curves[2] = newCurve(&voltmeter->data[0], &voltmeter->data[3], QColor("#729fcf"), false); // sky blue 0
    curves[3] = newCurve(&voltmeter->data[0], &voltmeter->data[4], QColor("#ef2929"), false); // scarlet red 0

    connect(voltmeter, SIGNAL(measurementDataChanged(bool)),
            this, SLOT(on_measurementDataChanged(bool)));
    connect(voltmeter, SIGNAL(unsavedDataChanged(bool)),
            this, SLOT(on_unsavedDataChanged(bool)));
    connect(voltmeter, SIGNAL(autoSaveChanged(bool)),
            this, SLOT(on_autoSaveChanged(bool)));
    connect(voltmeter, SIGNAL(fileNameChanged(QString)),
            this, SLOT(on_fileNameChanged(QString)));
    connect(voltmeter, SIGNAL(channelsChanged(std::bitset<4>)),
            this, SLOT(on_channelsChanged(std::bitset<4>)));

    connect(voltmeter, SIGNAL(replot()),
            this, SLOT(on_replot()));
}

QwtPlotCurve *
LoggerForm::newCurve(model::MinMaxVector *time, model::MinMaxVector *value, const QColor &color, bool visible)
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

QwtPlotGrid *
LoggerForm::newGrid()
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
LoggerForm::on_startButton_clicked()
{
    if (!voltmeter->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(voltmeter)) return;
        }
        voltmeter->start();
    }
}

void
LoggerForm::on_stopButton_clicked()
{
    if (voltmeter->active()) {
        voltmeter->stop();
    }
}

void
LoggerForm::on_ch1CheckBox_stateChanged(int state)
{
    auto channels = voltmeter->channels();
    channels[0] = (state == Qt::Checked);
    voltmeter->setChannels(channels);
}

void
LoggerForm::on_ch2CheckBox_stateChanged(int state)
{
    auto channels = voltmeter->channels();
    channels[1] = (state == Qt::Checked);
    voltmeter->setChannels(channels);
}

void
LoggerForm::on_ch3CheckBox_stateChanged(int state)
{
    auto channels = voltmeter->channels();
    channels[2] = (state == Qt::Checked);
    voltmeter->setChannels(channels);
}

void
LoggerForm::on_ch4CheckBox_stateChanged(int state)
{
    auto channels = voltmeter->channels();
    channels[3] = (state == Qt::Checked);
    voltmeter->setChannels(channels);
}

void
LoggerForm::on_saveButton_clicked()
{
    save();
}

void
LoggerForm::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern");
    try {
        voltmeter->save(fileName);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
LoggerForm::on_clearButton_clicked()
{
    voltmeter->clear();
}

void
LoggerForm::on_autoSaveCheckBox_stateChanged(int state)
{
    if (state == Qt::Unchecked)
        voltmeter->setAutoSave(false);
    else if (state == Qt::Checked)
        voltmeter->setAutoSave(true);
}

void
LoggerForm::on_replot()
{
    ui->plot->replot();
}

void
LoggerForm::on_measurementDataChanged(bool measurementData)
{
    if (measurementData) {
        ui->intervalComboBox->setEnabled(false);
    }
    else {
        ui->intervalComboBox->setEnabled(true);
    }
}

void
LoggerForm::on_unsavedDataChanged(bool unsavedData)
{

}

void
LoggerForm::on_autoSaveChanged(bool autoSave)
{
    if (autoSave && ui->autoSaveCheckBox->checkState() == Qt::Unchecked)
        ui->autoSaveCheckBox->setCheckState(Qt::Checked);
    if (!autoSave && ui->autoSaveCheckBox->checkState() == Qt::Checked)
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
}

void
LoggerForm::on_fileNameChanged(const QString &fileName)
{
    ui->autoSaveEdit->setText(fileName);
    if (fileName.isEmpty()) {
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
        ui->autoSaveCheckBox->setEnabled(false);
    }
    else {
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
        ui->autoSaveCheckBox->setEnabled(true);
    }
}

void
LoggerForm::on_channelsChanged(const std::bitset<4> &channels)
{
    for (auto i = 0; i < 4; i++)
        curves[i]->setVisible(channels[i]);
    ui->plot->replot();
}

void
LoggerForm::on_intervalComboBox_activated(int index)
{
    static const int interval[] = {100, 200, 500, 1000, 2000, 5000};
    voltmeter->setInterval(interval[index]);
}

void
LoggerForm::saveImage()
{
    QwtPlotRenderer renderer;
    renderer.exportTo(ui->plot, "Logger_Graph");
}

} // namespace gui
