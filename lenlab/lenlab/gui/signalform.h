#ifndef GUI_SIGNALFORM_H
#define GUI_SIGNALFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include <QWidget>

namespace gui {

namespace Ui {
class SignalForm;
}

class SignalForm : public QWidget
{
    Q_OBJECT

public:
    explicit SignalForm(QWidget *parent = 0);
    ~SignalForm();

    void setMainWindow(MainWindow *_main_window);
    void setLenlab(model::Lenlab *_lenlab);
    void setTitle(QString title);

private:
    void setParameterUIConfiguration(bool amplitude, bool frequency, bool dutycycle);
    Ui::SignalForm *ui;
    MainWindow *main_window;
    model::Lenlab *lenlab;
};


} // namespace gui
#endif // GUI_SIGNALFORM_H
