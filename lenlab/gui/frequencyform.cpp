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
    chart->addAxis(axisX, Qt::AlignBottom);

    auto seriesM = new QLineSeries();
    seriesM->setName("Amplitude");
    chart->addSeries(seriesM);
    seriesM->attachAxis(axisX);

    QValueAxis *axisM = new QValueAxis();
    chart->addAxis(axisM, Qt::AlignLeft);
    seriesM->attachAxis(axisM);

    auto seriesPh = new QLineSeries();
    seriesPh->setName("Phase");
    chart->addSeries(seriesPh);
    seriesPh->attachAxis(axisX);

    QValueAxis *axisPh = new QValueAxis();
    chart->addAxis(axisPh, Qt::AlignRight);
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

    connect(&m_lenlab->voltmeter, &model::Voltmeter::activeChanged,
            this, &FrequencyForm::activeChanged);
    connect(&m_lenlab->oscilloscope, &model::Oscilloscope::activeChanged,
            this, &FrequencyForm::activeChanged);
}

void
FrequencyForm::on_startButton_clicked()
{
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
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern", "bode.csv", tr("CSV (*.csv)"));
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
    LabChart chart = LabChart();
    prepareChart(&chart);
    chart.chart()->legend()->show();
    chart.replace(m_frequencysweep->getSeries());
    chart.print("bode.pdf");
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
