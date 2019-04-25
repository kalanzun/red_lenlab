#include "factory.h"

#include "usb/exception.h"

#include "lenlab_version.h"

#include <QDebug>

namespace protocol {

Factory::Factory(QObject *parent) : QObject(parent)
{
    connect(&mTimer, &QTimer::timeout,
            this, &Factory::on_timeout);

    mTimer.setSingleShot(true);
}

void
Factory::start()
{
    Q_ASSERT(!mBoard);
    Q_ASSERT(!mTimer.isActive());

    on_timeout();
}

void
Factory::on_timeout()
{
    if (mBoard) return;

    try {
        auto device = mBus.query(LENLAB_VID, LENLAB_PID);
        if (device) {
            mBoard.reset(new Board(device));
            connect(mBoard.data(), &Board::error,
                    this, &Factory::on_board_error);
            pMessage cmd(new Message());
            cmd->setCommand(::init);
            auto init = mBoard->startTask(cmd);
            connect(init.data(), &Task::succeeded,
                    this, &Factory::on_init);
            connect(init.data(), &Task::failed,
                    this, &Factory::on_task_error);
        }
        else {
            mTimer.start(500);
        }
    } catch (const usb::Exception &e) {
        emit error(e.getMsg());
        mTimer.start(3000);
    }
}

void
Factory::on_board_error(QString const & msg)
{
    mBoard.clear();
    emit error(msg);
}

void
Factory::on_task_error(pTask const & task)
{
    Q_UNUSED(task);

    if (task->isTimeout())
        qDebug("Task timeout");

    mBoard.clear();
    emit error("Task fehlgeschlagen");
}

void
Factory::on_init(pTask const & task)
{
    Q_UNUSED(task);

    pMessage cmd(new Message());
    cmd->setCommand(::getName);
    auto name = mBoard->startTask(cmd);
    connect(name.data(), &Task::succeeded,
            this, &Factory::on_name);
    connect(name.data(), &Task::failed,
            this, &Factory::on_task_error);
}

void
Factory::on_name(pTask const & task)
{
    Q_UNUSED(task);

    pMessage cmd(new Message());
    cmd->setCommand(::getVersion);
    auto version = mBoard->startTask(cmd);
    connect(version.data(), &Task::succeeded,
            this, &Factory::on_version);
    connect(version.data(), &Task::failed,
            this, &Factory::on_task_error);
}

void
Factory::on_version(pTask const & task)
{
    auto reply = task->getReply();
    auto length = reply->getUInt32BufferLength();
    if (length == 3 || length == 2) {
        auto array = reply->getUInt32Buffer();

        auto major = array[0];
        auto minor = array[1];

        if (major == MAJOR && minor == MINOR) {
            disconnect(mBoard.data(), &Board::error,
                       this, &Factory::on_board_error);
            emit ready(mBoard);
        }
        else {
            auto msg = QString("Ungültige Version %1.%2. Lenlab erwartet mindestens %3.%4").arg(major).arg(minor).arg(MAJOR).arg(MINOR);
            qDebug() << msg;
            mBoard.clear();
            emit error(msg);
        }
    }
    else {
        auto msg = QString("Das Lenlab Board antwortet mit einer ungültigen Version");
        qDebug() << msg;
        mBoard.clear();
        emit error(msg);
    }
}

} // namespace protocol
