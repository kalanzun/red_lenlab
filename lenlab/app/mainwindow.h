#ifndef APP_MAINWINDOW_H
#define APP_MAINWINDOW_H

#include <QMainWindow>

namespace app {

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

} // namespace app
#endif // APP_MAINWINDOW_H
