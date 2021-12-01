#include "app/mainwindow.h"
#include "protocol/board.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    protocol::Board board;
    QTimer::singleShot(0, &board, &protocol::Board::lookForBoard);

    QApplication a(argc, argv);
    app::MainWindow w;
    w.show();
    return a.exec();
}
