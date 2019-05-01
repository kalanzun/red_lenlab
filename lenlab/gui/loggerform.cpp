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

LoggerForm::LoggerForm(QWidget * parent) :
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

    m_curves[0] = newCurve(QColor("#729fcf"), true); // sky blue 0
    m_curves[1] = newCurve(QColor("#8ae234"), false); // green 0
    m_curves[2] = newCurve(QColor("#ef2929"), false); // scarlet red 0
    m_curves[3] = newCurve(QColor("#fce94f"), false); // butter 0

    newGrid();
}

LoggerForm::~LoggerForm()
{
    qDebug() << "~LoggerForm";
    delete ui;
}

QwtPlotCurve *
LoggerForm::newCurve(QColor const & color, bool visible)
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
LoggerForm::setMainWindow(MainWindow * main_window)
{
    m_main_window = main_window;
}

void
LoggerForm::setModel(model::Lenlab * lenlab)
{
    m_lenlab = lenlab;
    m_voltmeter = &lenlab->voltmeter;

    connect(m_voltmeter, &model::Voltmeter::seriesChanged,
            this, &LoggerForm::seriesChanged);
    connect(m_voltmeter, &model::Voltmeter::seriesUpdated,
            this, &LoggerForm::seriesUpdated);

    seriesChanged(m_voltmeter->getSeries());

    connect(m_voltmeter, &model::Voltmeter::measurementDataChanged,
            this, &LoggerForm::on_measurementDataChanged);
    connect(m_voltmeter, &model::Voltmeter::unsavedDataChanged,
            this, &LoggerForm::on_unsavedDataChanged);
    connect(m_voltmeter, &model::Voltmeter::autoSaveChanged,
            this, &LoggerForm::on_autoSaveChanged);
    connect(m_voltmeter, &model::Voltmeter::fileNameChanged,
            this, &LoggerForm::on_fileNameChanged);
    connect(m_voltmeter, &model::Voltmeter::channelsChanged,
            this, &LoggerForm::on_channelsChanged);
}

void
LoggerForm::on_startButton_clicked()
{
    if (!m_voltmeter->active()) {
        if (m_lenlab->isActive()) {
            if (!m_main_window->askToCancelActiveComponent(m_voltmeter)) return;
        }
        m_voltmeter->start();
    }
}

void
LoggerForm::on_stopButton_clicked()
{
    if (m_voltmeter->active()) {
        m_voltmeter->stop();
    }
}

void
LoggerForm::on_ch1CheckBox_stateChanged(int state)
{
    auto channels = m_voltmeter->channels();
    channels[0] = (state == Qt::Checked);
    m_voltmeter->setChannels(channels);
}

void
LoggerForm::on_ch2CheckBox_stateChanged(int state)
{
    auto channels = m_voltmeter->channels();
    channels[1] = (state == Qt::Checked);
    m_voltmeter->setChannels(channels);
}

void
LoggerForm::on_ch3CheckBox_stateChanged(int state)
{
    auto channels = m_voltmeter->channels();
    channels[2] = (state == Qt::Checked);
    m_voltmeter->setChannels(channels);
}

void
LoggerForm::on_ch4CheckBox_stateChanged(int state)
{
    auto channels = m_voltmeter->channels();
    channels[3] = (state == Qt::Checked);
    m_voltmeter->setChannels(channels);
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
        m_voltmeter->save(fileName);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
LoggerForm::on_clearButton_clicked()
{
    m_voltmeter->clear();
}

void
LoggerForm::on_autoSaveCheckBox_stateChanged(int state)
{
    if (state == Qt::Unchecked)
        m_voltmeter->setAutoSave(false);
    else if (state == Qt::Checked)
        m_voltmeter->setAutoSave(true);
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
    Q_UNUSED(unsavedData);
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
    for (std::size_t i = 0; i < m_curves.size(); ++i)
        m_curves[i]->setVisible(channels[i]);
    ui->plot->replot();
}

void
LoggerForm::on_intervalComboBox_activated(int index)
{
    static uint32_t const interval[] = {100, 200, 500, 1000, 2000, 5000};
    m_voltmeter->setInterval(interval[index]);
}

void
LoggerForm::saveImage()
{
    QwtPlotRenderer renderer;
    renderer.exportTo(ui->plot, "Logger_Graph");
}

void
LoggerForm::seriesChanged(model::pSeries const & series)
{
    for (unsigned int i = 0; i < m_curves.size(); ++i) {
        m_curves[i]->setSamples(new PointVectorSeriesData(series, i)); // acquires ownership
    }
    ui->plot->replot();
}

void
LoggerForm::seriesUpdated()
{
    ui->plot->replot();
}

} // namespace gui
