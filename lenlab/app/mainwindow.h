#ifndef APP_MAINWINDOW_H
#define APP_MAINWINDOW_H

#include <QMainWindow>

namespace model {

class Lenlab;

}

namespace app {

class LoggerForm;
class OscilloscopeForm;
class SpectrumForm;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

    model::Lenlab* lenlab;

    LoggerForm* logger_form;
    OscilloscopeForm* oscilloscope_form;
    SpectrumForm* spectrum_form;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setModel(model::Lenlab* lenlab);

private:
    Ui::MainWindow *ui;
};

} // namespace app
#endif // APP_MAINWINDOW_H
