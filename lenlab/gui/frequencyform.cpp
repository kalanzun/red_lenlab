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

#include "frequencyform.h"
#include "ui_frequencyform.h"

#include <QFileDialog>
#include <QMessageBox>

namespace gui {

FrequencyForm::FrequencyForm(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::FrequencyForm)
{
    QString stylesheet;

    ui->setupUi(this);

    prepareChart(ui->labChart);

    auto series = ui->labChart->series();

    stylesheet += "#ch1Label { color: "
            + series.at(0)->color().name() + "; }\n";
    stylesheet += "#ch2Label { color: "
            + series.at(1)->color().name() + "; }\n";
    stylesheet += "#magnitudeLabel { color: "
            + series.at(0)->color().name() + "; }\n";
    stylesheet += "#phaseLabel { color: "
            + series.at(1)->color().name() + "; }\n";

    ui->scrollAreaWidgetContents->setStyleSheet(stylesheet);
}

FrequencyForm::~FrequencyForm()
{
    delete ui;
}

void
FrequencyForm::prepareChart(LabChart *chart)
{
    chart->setLabelX("Frequenz [Hz]");
    chart->setLabelY("Amplitude [dB]");
    chart->setLabelY2("Phase [°]");

    QLogValueAxis *axisX = new QLogValueAxis();
    // Note: With kHz, from 0.1 kHz it has rounding issues and the max tick is missing
    axisX->setBase(10);
    axisX->setLabelFormat("%g");
    chart->addAxis(axisX, Qt::AlignBottom);

    QValueAxis *axisM = new QValueAxis();
    chart->addAxis(axisM, Qt::AlignLeft);

    QValueAxis *axisPh = new QValueAxis();
    chart->addAxis(axisPh, Qt::AlignRight);

    auto seriesM = new QLineSeries();
    seriesM->setName("Amplitude");
    chart->addSeries(seriesM);
    seriesM->attachAxis(axisX);
    seriesM->attachAxis(axisM);

    auto seriesPh = new QLineSeries();
    seriesPh->setName("Phase");
    chart->addSeries(seriesPh);
    seriesPh->attachAxis(axisX);
    seriesPh->attachAxis(axisPh);
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

    seriesChanged(m_frequencysweep->getSeries());

    connect(&m_lenlab->voltmeter, &model::Voltmeter::activeChanged,
            this, &FrequencyForm::activeChanged);
    connect(&m_lenlab->oscilloscope, &model::Oscilloscope::activeChanged,
            this, &FrequencyForm::activeChanged);
}

void
FrequencyForm::on_startButton_clicked()
{
    if (m_lenlab->isOpen()) {
        if (!m_frequencysweep->active()) {
            if (m_lenlab->isActive()) {
                if (m_main_window->askToCancelActiveComponent(m_frequencysweep)) {
                    if (m_lenlab->isActive()) {
                        // the component might have stopped while the dialog was visible
                        pending = true;
                        m_lenlab->getActiveComponent()->stop();
                    } else {
                        m_frequencysweep->start();
                    }
                }
            } else {
                m_frequencysweep->start();
            }
        }
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

void FrequencyForm::activeChanged(bool)
{
    if (pending) {
        pending = false;
        if (!m_lenlab->isActive()) m_frequencysweep->start();
    }
}

void
FrequencyForm::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern", "bode.csv", "CSV (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QSaveFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Speichern", QString("Fehler beim Speichern der Daten\n") + file.errorString());
        return;
    }

    QTextStream stream(&file);
    m_frequencysweep->save(stream);
    file.commit();
}

void
FrequencyForm::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Bild Speichern", "bode.pdf", "PDF (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    LabChart chart;
    prepareChart(&chart);
    chart.chart()->legend()->show();
    chart.replace(m_frequencysweep->getSeries());
    chart.print(fileName);
}

void
FrequencyForm::seriesUpdated(model::pSeries const & series)
{
    ui->labChart->appendLast(series);
}

void
FrequencyForm::seriesChanged(model::pSeries const & series)
{
    ui->labChart->replace(series);
}

} // namespace gui
