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

#include "loggerform.h"
#include "ui_loggerform.h"

#include <QMessageBox>
#include <QFileDialog>

namespace gui {

LoggerForm::LoggerForm(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    QString stylesheet;

    ui->setupUi(this);

    ui->autoSaveCheckBox->setEnabled(false);

    prepareChart(ui->labChart);

    auto series = ui->labChart->series();
    for (unsigned int i = 0; i < series.size(); ++i) {
        stylesheet += "#ch" + QString::number(i + 1) + "CheckBox { color: "
                + series.at(i)->color().name() + "; }\n";
        series.at(i)->setVisible(i == 0);
    }

    ui->scrollAreaWidgetContents->setStyleSheet(stylesheet);
}

LoggerForm::~LoggerForm()
{
    delete ui;
}

void
LoggerForm::prepareChart(LabChart *chart)
{
    chart->setLabelX("Zeit [s]");
    chart->setLabelY("Spannung [V]");

    for (unsigned int i = 0; i < 4; ++i) {
        chart->addSeries(new QLineSeries());
    }

    chart->createDefaultAxes();
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
            this, &LoggerForm::measurementDataChanged);
    connect(m_voltmeter, &model::Voltmeter::unsavedDataChanged,
            this, &LoggerForm::unsavedDataChanged);
    connect(m_voltmeter, &model::Voltmeter::autoSaveChanged,
            this, &LoggerForm::autoSaveChanged);
    connect(m_voltmeter, &model::Voltmeter::fileNameChanged,
            this, &LoggerForm::fileNameChanged);
    connect(m_voltmeter, &model::Voltmeter::channelsChanged,
            this, &LoggerForm::channelsChanged);

    connect(&m_lenlab->oscilloscope, &model::Oscilloscope::activeChanged,
            this, &LoggerForm::activeChanged);
    connect(&m_lenlab->frequencysweep, &model::Oscilloscope::activeChanged,
            this, &LoggerForm::activeChanged);
}

void
LoggerForm::on_startButton_clicked()
{
    if (!m_voltmeter->active()) {
        if (m_lenlab->isActive()) {
            if (m_main_window->askToCancelActiveComponent(m_voltmeter)) {
                if (m_lenlab->isActive()) {
                    // the component might have stopped while the dialog was visible
                    pending = true;
                    m_lenlab->getActiveComponent()->stop();
                } else {
                    m_voltmeter->start();
                }
            }
        } else {
            m_voltmeter->start();
        }
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
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern", "logger.csv", tr("CSV (*.csv)"));
    try {
        m_voltmeter->save(fileName);
    }
    catch (std::exception const &) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
LoggerForm::saveImage()
{
    LabChart chart = LabChart();
    prepareChart(&chart);

    auto channels = m_voltmeter->channels();
    for (unsigned int i = 0; i < 4; ++i)
        chart.setChannelVisible(i, channels[i]);

    chart.replace(m_voltmeter->getSeries());
    chart.print("logger.pdf");
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
LoggerForm::measurementDataChanged(bool measurementData)
{
    if (measurementData) {
        ui->intervalComboBox->setEnabled(false);
    }
    else {
        ui->intervalComboBox->setEnabled(true);
    }
}

void
LoggerForm::unsavedDataChanged(bool unsavedData)
{
    Q_UNUSED(unsavedData);
}

void
LoggerForm::autoSaveChanged(bool autoSave)
{
    if (autoSave && ui->autoSaveCheckBox->checkState() == Qt::Unchecked)
        ui->autoSaveCheckBox->setCheckState(Qt::Checked);
    if (!autoSave && ui->autoSaveCheckBox->checkState() == Qt::Checked)
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
}

void
LoggerForm::fileNameChanged(const QString &fileName)
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
LoggerForm::channelsChanged(const std::bitset<4> &channels)
{
    for (unsigned int i = 0; i < 4; ++i)
        ui->labChart->setChannelVisible(i, channels[i]);
}

void LoggerForm::activeChanged(bool)
{
    if (pending) {
        pending = false;
        if (!m_lenlab->isActive()) m_voltmeter->start();
    }
}

void
LoggerForm::on_intervalComboBox_activated(int index)
{
    static uint32_t const interval[] = {100, 200, 500, 1000, 2000, 5000};
    m_voltmeter->setInterval(interval[index]);
}

void
LoggerForm::seriesChanged(model::pSeries const & series)
{
    ui->labChart->replace(series);
}

void
LoggerForm::seriesUpdated(model::pSeries const & series)
{
    ui->labChart->appendLast(series);
}

} // namespace gui
