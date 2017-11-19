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
    if (!logger->isActive()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(logger)) return;
        }
        logger->start();
    }
}

void
LoggerForm::on_stopButton_clicked()
{
    if (logger->isActive()) {
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
        ui->autoSaveEdit->setText(fileName);
        ui->autoSaveCheckBox->setEnabled(true);
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
        ui->autoSaveEdit->setText(QString());
        ui->autoSaveCheckBox->setEnabled(false);
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
    }
}

void
LoggerForm::on_clearButton_clicked()
{
    ui->autoSaveEdit->setText(QString());
    ui->autoSaveCheckBox->setEnabled(false);
    ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);

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
LoggerForm::on_autoSaveChanged(bool autoSave)
{
    if (autoSave && ui->autoSaveCheckBox->checkState() == Qt::Unchecked)
        ui->autoSaveCheckBox->setCheckState(Qt::Checked);
    if (!autoSave && ui->autoSaveCheckBox->checkState() == Qt::Checked)
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
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
