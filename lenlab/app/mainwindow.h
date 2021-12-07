#ifndef APP_MAINWINDOW_H
#define APP_MAINWINDOW_H

#include <QMainWindow>

#include "model/lenlab.h"
#include "loggerform.h"
#include "oscilloscopeform.h"
#include "spectrumform.h"

namespace app {

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
