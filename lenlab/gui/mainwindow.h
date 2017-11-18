#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include "model/lenlab.h"
#include "usb/handler.h"
#include <QMainWindow>

namespace gui {

namespace Ui {
class MainWindow;
}

/**
 * @brief Lenlab main window.
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setModel(model::Lenlab *lenlab);
    void setHandler(usb::Handler *handler);

    bool askToCancelActiveComponent(model::Component *next_component);

private slots:
    void on_replot();
    void on_signalAButton_toggled(bool);
    void on_signalBButton_toggled(bool);
    void on_logButton_toggled(bool);
    void on_toolBox_currentChanged(int);
    void on_logMessage(const QString &);

private:
    Ui::MainWindow *ui;
    model::Lenlab *lenlab;
    usb::Handler *handler;

    bool signalA_checked = false;
    bool signalB_checked = false;
};

} // namespace gui
#endif // GUI_MAINWINDOW_H
