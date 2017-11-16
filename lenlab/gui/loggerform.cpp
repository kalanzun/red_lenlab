#include "loggerform.h"
#include "ui_loggerform.h"
#include "pointvectorseriesdata.h"
#include "mainwindow.h"
#include <QMessageBox>

namespace gui {

LoggerForm::LoggerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoggerForm)
{
    ui->setupUi(this);
}

LoggerForm::~LoggerForm()
{
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
    curve->attach(plot);
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

} // namespace gui
