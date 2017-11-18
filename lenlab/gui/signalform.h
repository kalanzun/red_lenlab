#ifndef GUI_SIGNALFORM_H
#define GUI_SIGNALFORM_H

#include "mainwindow.h"
#include "model/lenlab.h"
#include <QWidget>

namespace gui {

namespace Ui {
class SignalForm;
}

/**
 * @brief Controls for the Lenlab Signal Generator component.
 */

class SignalForm : public QWidget
{
    Q_OBJECT

public:
    explicit SignalForm(QWidget *parent = 0);
    ~SignalForm();

    void setMainWindow(MainWindow *main_window);
    void setModel(model::Lenlab *lenlab);
    void setTitle(QString title);

private:
    void setUIConfiguration(bool amplitude, bool frequency, bool dutycycle);

    Ui::SignalForm *ui;

    MainWindow *main_window;
    model::Lenlab *lenlab;
    model::Signal *signal;
};

} // namespace gui
#endif // GUI_SIGNALFORM_H
