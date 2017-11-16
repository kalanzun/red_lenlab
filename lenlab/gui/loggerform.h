#ifndef GUI_LOGGERFORM_H
#define GUI_LOGGERFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include "model/minmaxvector.h"
#include "qwt_plot_curve.h"
#include <QWidget>

namespace gui {

namespace Ui {
class LoggerForm;
}

/**
 * @brief Controls for the Lenlab Logger component.
 */

class LoggerForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoggerForm(QWidget *parent = 0);
    ~LoggerForm();

    void setMainWindow(MainWindow *_main_window);
    void setLenlab(model::Lenlab *_lenlab);

    void configurePlot(QwtPlot *plot);
    void configureCurve(QwtPlot *plot, QwtPlotCurve *curve, model::MinMaxVector *time, model::MinMaxVector *value, const QColor &color, qreal width, bool visible);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::LoggerForm *ui;
    MainWindow *main_window;
    model::Lenlab *lenlab;

    std::array<QwtPlotCurve, 4> curves;
};


} // namespace gui
#endif // GUI_LOGGERFORM_H
