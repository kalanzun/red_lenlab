#ifndef APP_MAINWINDOW_H
#define APP_MAINWINDOW_H

#include <QMainWindow>

namespace model {

class Lenlab;

} // namespace model

namespace app {

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

    model::Lenlab* lenlab;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setModel(model::Lenlab* lenlab);

private:
    Ui::MainWindow *ui;
};

} // namespace app
#endif // APP_MAINWINDOW_H
