#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>

namespace gui {

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_signalAButton_toggled(bool checked);
    void on_signalBButton_toggled(bool checked);
    void on_logButton_toggled(bool checked);
    void on_toolBox_currentChanged(int index);

private:
    Ui::MainWindow *ui;

    bool signalA_checked = false;
    bool signalB_checked = false;
};


} // namespace gui
#endif // GUI_MAINWINDOW_H
