#ifndef FACTORY_H
#define FACTORY_H

#include "board.h"
#include "message.h"

#include "usb/bus.h"

#include <QObject>
#include <QTimer>

namespace protocol {

class Factory : public QObject
{
    Q_OBJECT

    static int const mPollTime = 500;
    static int const mBootTime = 1200;
    static int const mErrorTime = 3000;

    QTimer mTimer;
    QTimer mBootTimer;
    usb::Bus mBus;
    pBoard mBoard;

public:
    explicit Factory(QObject *parent = nullptr);
    Factory(Factory const & other) = delete;

    Factory & operator=(Factory const & other) = delete;

    void connectToBoard(int boottime = 0);

signals:
    void ready(pBoard const &);
    void log(QString const &);
    void error(QString const &);

private slots:
    void on_board_destroyed();
    void on_timeout();
    void on_boot();
    void on_board_error(QString const &);
    void on_task_error(pTask const &);
    void on_init(pTask const &);
    void on_name(pTask const &);
    void on_version(pTask const &);
};

} // namespace protocol

#endif // FACTORY_H
