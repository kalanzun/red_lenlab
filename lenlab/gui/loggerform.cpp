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

void
LoggerForm::setMainWindow(MainWindow *_main_window)
{
    main_window = _main_window;
}

void
LoggerForm::setLenlab(model::Lenlab *_lenlab)
{
    lenlab = _lenlab;
    /*
    connect(lenlab->logger, SIGNAL(autoSaveChanged(bool)),
            this, SLOT(on_autoSaveChanged(bool)));
            */
}

void
LoggerForm::configurePlot(QwtPlot *plot)
{
    configureCurve(plot, &curves[0], &lenlab->logger->data[0], &lenlab->logger->data[1], Qt::yellow, 2, true);
    configureCurve(plot, &curves[1], &lenlab->logger->data[0], &lenlab->logger->data[2], Qt::green, 2, false);
    configureCurve(plot, &curves[2], &lenlab->logger->data[0], &lenlab->logger->data[3], Qt::blue, 2, false);
    configureCurve(plot, &curves[3], &lenlab->logger->data[0], &lenlab->logger->data[4], Qt::red, 2, false);
}

void
LoggerForm::configureCurve(QwtPlot *plot, QwtPlotCurve *curve, model::MinMaxVector *time, model::MinMaxVector *value, const QColor &color, qreal width, bool visible)
{
    curve->setSamples(new PointVectorSeriesData(time, value)); // acquires ownership
    curve->setPen(color, width);
    curve->setVisible(visible);
    //curve->attach(plot);
}

void
LoggerForm::on_startButton_clicked()
{
    if (!lenlab->logger->isActive()) {
        if (lenlab->isActive()) {
            if (!main_window->askToCancelActiveComponent(lenlab->logger)) return;
        }
        lenlab->logger->start();
    }
}

void
LoggerForm::on_stopButton_clicked()
{
    if (lenlab->logger->isActive()) {
        lenlab->logger->stop();
    }
}

void
LoggerForm::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Speichern");
    try {
        lenlab->logger->save(fileName);
        ui->autoSaveLineEdit->setText(fileName);
        ui->autoSaveCheckBox->setEnabled(true);
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
    }
    catch (std::exception) {
        QMessageBox::critical(this, "Speichern", "Fehler beim Speichern der Daten"); // TODO include reason
        ui->autoSaveLineEdit->setText(QString());
        ui->autoSaveCheckBox->setEnabled(false);
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
    }
}

void
LoggerForm::on_clearButton_clicked()
{
    ui->autoSaveLineEdit->setText(QString());
    ui->autoSaveCheckBox->setEnabled(false);
    ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
    lenlab->logger->clear();
}

void
LoggerForm::on_autoSaveCheckBox_stateChanged(int state)
{
    if (state == Qt::Unchecked)
        lenlab->logger->setAutoSave(false);
    else if (state == Qt::Checked)
        lenlab->logger->setAutoSave(true);
}

void
LoggerForm::on_autoSaveChanged(bool autoSave)
{
    if (autoSave && ui->autoSaveCheckBox->checkState() == Qt::Unchecked)
        ui->autoSaveCheckBox->setCheckState(Qt::Checked);
    if (!autoSave && ui->autoSaveCheckBox->checkState() == Qt::Checked)
        ui->autoSaveCheckBox->setCheckState(Qt::Unchecked);
}

} // namespace gui
