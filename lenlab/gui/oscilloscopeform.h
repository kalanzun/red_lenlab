#ifndef GUI_OSCILLOSCOPEFORM_H
#define GUI_OSCILLOSCOPEFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include <QWidget>

namespace gui {

namespace Ui {
class OscilloscopeForm;
}

/**
 * @brief Controls for the Lenlab oscilloscope component.
 */

class OscilloscopeForm : public QWidget
{
    Q_OBJECT

public:
    explicit OscilloscopeForm(QWidget *parent = 0);
    ~OscilloscopeForm();

    void setMainWindow(MainWindow *main_window);
    void setModel(model::Lenlab *lenlab);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::OscilloscopeForm *ui;

    MainWindow *main_window;
    model::Lenlab *lenlab;
    model::Oscilloscope *oscilloscope;
};


} // namespace gui
#endif // GUI_OSCILLOSCOPEFORM_H
