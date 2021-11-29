#include "mainwindow.h"
#include "app/ui_mainwindow.h"

namespace app {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


} // namespace app
