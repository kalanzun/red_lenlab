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

    void setMainWindow(MainWindow *main_window);
    void setModel(model::Lenlab *lenlab);

private slots:
    void on_intervalButton_clicked();
    void on_intervalEdit_editingFinished();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_ch1CheckBox_stateChanged(int);
    void on_ch2CheckBox_stateChanged(int);
    void on_ch3CheckBox_stateChanged(int);
    void on_ch4CheckBox_stateChanged(int);
    void on_saveButton_clicked();
    void on_autoSaveCheckBox_stateChanged(int);
    void on_clearButton_clicked();

    void on_replot();
    void on_measurementDataChanged(bool);
    void on_unsavedDataChanged(bool);
    void on_autoSaveChanged(bool);
    void on_fileNameChanged(const QString &);

private:
    QwtPlotCurve *newCurve(model::MinMaxVector *time, model::MinMaxVector *value, const QColor &color, qreal width, bool visible);

    Ui::LoggerForm *ui;

    MainWindow *main_window;
    model::Lenlab *lenlab;
    model::Logger *logger;

    std::array<QwtPlotCurve *, 4> curves; // pointer, no ownership
};


} // namespace gui
#endif // GUI_LOGGERFORM_H
