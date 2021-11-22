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

#include "oscilloscopeform.h"
#include "ui_oscilloscopeform.h"

#include <QFileDialog>
#include <QMessageBox>

namespace gui {

OscilloscopeForm::OscilloscopeForm(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::OscilloscopeForm)
{
    QString stylesheet;

    ui->setupUi(this);

    prepareChart(ui->labChart);

    auto series = ui->labChart->series();
    for (int i = 0; i < series.size(); ++i) {
        stylesheet += "#ch" + QString::number(i + 1) + "CheckBox { color: "
                + series.at(i)->color().name() + "; }\n";
        series.at(i)->setVisible(i == 0);
    }

    ui->scrollAreaWidgetContents->setStyleSheet(stylesheet);
}

OscilloscopeForm::~OscilloscopeForm()
{
    delete ui;
}

void
OscilloscopeForm::prepareChart(LabChart *chart)
{
    chart->setLabelX("Zeit [ms]");
    chart->setLabelY("Spannung [V]");

    for (unsigned int i = 0; i < 2; ++i) {
        auto series = new QLineSeries();
        series->setName(QString("Kanal ") + QString::number(i + 1));
        chart->addSeries(series);
    }

    chart->createDefaultAxes();
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

    for (int i = 0; i < m_oscilloscope->samplerate_count; ++i)
        ui->samplerateBox->addItem(m_oscilloscope->getSamplerateLabel(i));

    for (int i = 0; i < m_oscilloscope->view_count; ++i)
        ui->timerangeBox->addItem(m_oscilloscope->getViewLabel(i));

    connect(m_oscilloscope, &model::Oscilloscope::seriesChanged,
            this, &OscilloscopeForm::seriesChanged);

    seriesChanged(m_oscilloscope->getSeries());

    connect(&m_lenlab->voltmeter, &model::Voltmeter::activeChanged,
            this, &OscilloscopeForm::activeChanged);
    connect(&m_lenlab->frequencysweep, &model::Oscilloscope::activeChanged,
            this, &OscilloscopeForm::activeChanged);
}

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
    ui->labChart->replace(series);

    // samplerate may be different
    if (m_oscilloscope->getSamplerateIndex() != m_current_samplerate_index) {
        m_current_samplerate_index = m_oscilloscope->getSamplerateIndex();

        // update timerangeBox
        for (int i = 0; i < m_oscilloscope->view_count; ++i)
            ui->timerangeBox->setItemText(i, m_oscilloscope->getViewLabel(i));
    }

}

void
OscilloscopeForm::on_samplerateBox_activated(int index)
{
    m_oscilloscope->setSamplerateIndex(index);
}

void
OscilloscopeForm::on_ch1CheckBox_stateChanged(int state)
{
    ui->labChart->setChannelVisible(0, state == Qt::Checked);
}

void
OscilloscopeForm::on_ch2CheckBox_stateChanged(int state)
{
    ui->labChart->setChannelVisible(1, state == Qt::Checked);
}

void
OscilloscopeForm::on_saveButton_clicked()
{
    save();
}

void
OscilloscopeForm::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern", "oszilloskop.csv", "CSV (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QSaveFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Speichern", QString("Fehler beim Speichern der Daten\n") + file.errorString());
        return;
    }

    QTextStream stream(&file);
    m_oscilloscope->save(stream);
    file.commit();
}

void
OscilloscopeForm::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Bild Speichern", "oszilloskop.pdf", "PDF (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    LabChart chart;
    prepareChart(&chart);
    chart.chart()->legend()->show();
    chart.setChannelVisible(0, ui->ch1CheckBox->checkState() == Qt::Checked);
    chart.setChannelVisible(1, ui->ch2CheckBox->checkState() == Qt::Checked);
    chart.replace(m_oscilloscope->getSeries());
    chart.print(fileName);
}

void
OscilloscopeForm::on_timerangeBox_currentIndexChanged(int index)
{
    m_oscilloscope->setViewIndex(index);
}

void
OscilloscopeForm::activeChanged(bool)
{
    if (pending) {
        pending = false;
        if (!m_lenlab->isActive()) m_oscilloscope->start();
    }
}

} // namespace gui
