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

    QChart *chart = new QChart();
    chart->legend()->hide();

    QLogValueAxis *axisX = new QLogValueAxis();
    chart->addAxis(axisX, Qt::AlignBottom);

    for (size_t i = 0; i < m_series.size(); ++i) {
        m_series[i] = new QLineSeries();
        chart->addSeries(m_series[i]);
        m_series[i]->attachAxis(axisX);
        stylesheet += "#ch" + QString::number(i + 1) + "Label { color: "
                + m_series[i]->color().name() + "; }\n";
    }

    stylesheet += "#amplitudeLabel { color: "
            + m_series[0]->color().name() + "; }\n";

    stylesheet += "#phaseLabel { color: "
            + m_series[1]->color().name() + "; }\n";

    QValueAxis *axisMag = new QValueAxis();
    chart->addAxis(axisMag, Qt::AlignLeft);
    m_series[0]->attachAxis(axisMag);

    QValueAxis *axisAng = new QValueAxis();
    chart->addAxis(axisAng, Qt::AlignRight);
    m_series[1]->attachAxis(axisAng);

    ui->plot->setChart(chart);
    ui->plot->setRenderHint(QPainter::Antialiasing);

    ui->scrollArea->setStyleSheet(stylesheet);

    /*
    QString x_label("Frequenz [Hz]");

    QString y_label("Amplitude [dB]");

    QString phase_label("Phase [°]");
    */
}

FrequencyForm::~FrequencyForm()
{
    delete ui;
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
    /*
    QwtPlotRenderer renderer;
    renderer.exportTo(ui->plot, "bode.pdf"); // it asks for the filename
    */
}

void
FrequencyForm::seriesUpdated(model::pSeries const & series)
{
    for (unsigned int channel = 0; channel < m_series.size(); ++channel) {
        m_series[channel]->append(series->getLastX(), series->getLastY(channel + 1));
    }

    ui->plot->chart()->axes(Qt::Horizontal)[0]->setRange(series->getMinX(), series->getMaxX());
    ui->plot->chart()->axes(Qt::Vertical)[0]->setRange(series->getMinY(1), series->getMaxY(1));
    ui->plot->chart()->axes(Qt::Vertical)[1]->setRange(series->getMinY(2), series->getMaxY(2));
}

void
FrequencyForm::seriesChanged(model::pSeries const & series)
{
    // a new measurement just started, series is empty

    for (unsigned int channel = 0; channel < m_series.size(); ++channel) {
        m_series[channel]->clear();
    }

    ui->plot->chart()->axes(Qt::Horizontal)[0]->setRange(series->getMinX(), series->getMaxX());
    ui->plot->chart()->axes(Qt::Vertical)[0]->setRange(series->getMinY(1), series->getMaxY(1));
    ui->plot->chart()->axes(Qt::Vertical)[1]->setRange(series->getMinY(2), series->getMaxY(2));
}

} // namespace gui
