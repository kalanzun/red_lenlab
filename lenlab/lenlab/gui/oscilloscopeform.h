#ifndef GUI_OSCILLOSCOPEFORM_H
#define GUI_OSCILLOSCOPEFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include <QWidget>

namespace gui {

namespace Ui {
class OscilloscopeForm;
}

class OscilloscopeForm : public QWidget
{
    Q_OBJECT

public:
    explicit OscilloscopeForm(QWidget *parent = 0);
    ~OscilloscopeForm();

    void setMainWindow(MainWindow *_main_window);
    void setLenlab(model::Lenlab *_lenlab);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::OscilloscopeForm *ui;
    MainWindow *main_window;
    model::Lenlab *lenlab;
};


} // namespace gui
#endif // GUI_OSCILLOSCOPEFORM_H
