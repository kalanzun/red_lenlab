#include "app/mainwindow.h"
#include "protocol/manager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    protocol::Manager manager;
    QTimer::singleShot(0, &manager, &protocol::Manager::lookForBoard);

    QApplication a(argc, argv);
    app::MainWindow w;
    w.show();
    return a.exec();
}
