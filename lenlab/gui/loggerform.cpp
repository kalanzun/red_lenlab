#include "loggerform.h"
#include "ui_loggerform.h"
#include "pointvectorseriesdata.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

namespace gui {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    qDebug() << "LoggerForm";
    ui->setupUi(this);

    ui->autoSaveCheckBox->setEnabled(false);
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
    this->logger = lenlab->logger;

    curves[0] = newCurve(&logger->data[0], &logger->data[1], Qt::yellow, 2, true);
    curves[1] = newCurve(&logger->data[0], &logger->data[2], Qt::green, 2, false);
    curves[2] = newCurve(&logger->data[0], &logger->data[3], Qt::blue, 2, false);
    curves[3] = newCurve(&logger->data[0], &logger->data[4], Qt::red, 2, false);

    connect(logger, SIGNAL(measurementDataChanged(bool)),
            this, SLOT(on_measurementDataChanged(bool)));
    connect(logger, SIGNAL(unsavedDataChanged(bool)),
            this, SLOT(on_unsavedDataChanged(bool)));
    connect(logger, SIGNAL(autoSaveChanged(bool)),
            this, SLOT(on_autoSaveChanged(bool)));
    connect(logger, SIGNAL(fileNameChanged(QString)),
            this, SLOT(on_fileNameChanged(QString)));

    connect(logger, SIGNAL(replot()),
            this, SLOT(on_replot()));
}

QwtPlotCurve *
LoggerForm::newCurve(model::MinMaxVector *time, model::MinMaxVector *value, const QColor &color, qreal width, bool visible)
{
    // Todo this may leak curves
    std::unique_ptr<QwtPlotCurve> curve(new QwtPlotCurve());
    curve->setSamples(new PointVectorSeriesData(time, value)); // acquires ownership
    curve->setPen(color, width);
    curve->setVisible(visible);
    curve->attach(ui->plot); // acquires ownership

    return curve.release();
}

void
LoggerForm::on_startButton_clicked()
{
    if (!logger->active()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(logger)) return;
        }
        logger->start();
    }
}

void
LoggerForm::on_stopButton_clicked()
{
    if (logger->active()) {
        logger->stop();
    }
}

void
LoggerForm::on_ch1CheckBox_stateChanged(int state)
{
    curves[0]->setVisible(state != 0);
    ui->plot->replot();
}

void
LoggerForm::on_ch2CheckBox_stateChanged(int state)
{
    curves[1]->setVisible(state != 0);
    ui->plot->replot();
}

void
LoggerForm::on_ch3CheckBox_stateChanged(int state)
{
    curves[2]->setVisible(state != 0);
    ui->plot->replot();
}

void
LoggerForm::on_ch4CheckBox_stateChanged(int state)
{
    curves[3]->setVisible(state != 0);
    ui->plot->replot();
}

void
LoggerForm::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern");
    try {
        logger->save(fileName);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
    }
}

void
LoggerForm::on_clearButton_clicked()
{
    logger->clear();
}

void
LoggerForm::on_autoSaveCheckBox_stateChanged(int state)
{
    if (state == Qt::Unchecked)
        logger->setAutoSave(false);
    else if (state == Qt::Checked)
        logger->setAutoSave(true);
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
        ui->intervalButton->setEnabled(false);
        ui->intervalEdit->setEnabled(false);
    }
    else {
        ui->intervalButton->setEnabled(true);
        ui->intervalEdit->setEnabled(true);
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
LoggerForm::on_intervalButton_clicked()
{

}

void
LoggerForm::on_intervalEdit_editingFinished()
{

}

} // namespace gui
