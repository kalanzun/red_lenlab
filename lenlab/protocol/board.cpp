/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "board.h"

#include "usb/usbexception.h"

#include "lenlab_version.h"

namespace protocol {

static int p_board_type_id = qRegisterMetaType<pBoard>("pBoard");

Board::Board(QObject *parent)
    : QObject(parent)
{
    connect(&mPollTimer, &QTimer::timeout,
            this, &Board::on_poll_timeout);
    connect(&mBootTimer, &QTimer::timeout,
            this, &Board::on_boot_timeout);
    connect(&mWatchdog, &QTimer::timeout,
            this, &Board::on_watchdog_timeout);

    mPollTimer.setSingleShot(true);
    mBootTimer.setSingleShot(true);
    mWatchdog.setSingleShot(true);
}

void
Board::lookForBoard(int boottime)
{
    Q_ASSERT(!mDevice);

    try {
        mDevice = mBus.query(LENLAB_VID, LENLAB_PID);
        if (mDevice) {
            connect(mDevice.data(), &usb::Device::reply,
                    this, &Board::on_reply);
            connect(mDevice.data(), &usb::Device::error,
                    this, &Board::on_error);
            connect(mDevice.data(), &usb::Device::destroyed,
                    this, &Board::on_destroyed);
            emit log("Lenlab-Board gefunden.");
            // wait for the board to boot, it just got power
            mBootTimer.start(boottime);
        } else {
            mPollTimer.start(mPollTime);
        }
    } catch (usb::USBException const & e) {
        emit error(e.msg());
        mPollTimer.start(mErrorTime);
    }
}

bool
Board::isOpen() const
{
    return (bool) mDevice;
}

bool
Board::isReady() const
{
    // TODO rename ready signal open, maybe implement closed signal
    return mDevice && !mTask;
}

void
Board::startTask(pTask const & task)
{
    if (!mDevice) {
        task->setError("Device not connected");
        emit task->failed(task);
        return;
    }

    if (mTask) {
        task->setError("Device busy");
        emit task->failed(task);
        return;
    }

    mTask = task;

    mDevice->send(task->getCommand()->getPacket());
    mWatchdog.start(task->getTimeout());
}

void
Board::queueTask(pTask const & task)
{
    mTaskQueue.append(task);
    sendFromQueue();
}



uint32_t
Board::getVersionMajor() const
{
    return mMajor;
}

uint32_t
Board::getVersionMinor() const
{
    return mMinor;
}

void
Board::sendFromQueue()
{
    if (!mTaskQueue.isEmpty() && !mTask) {
        pTask task = mTaskQueue.takeFirst();
        startTask(task);
    }
}

void Board::clearQueue()
{
    for (auto task: mTaskQueue) {
        task->setError("Clear task queue");
        emit task->failed(task);
    }
    mTaskQueue.clear();
}

void
Board::on_reply(usb::pPacket const & packet)
{
    auto task = mTask;
    auto message = pMessage::create(packet);

    if (message->getReply() == LoggerData) {
        emit logger_data(message);
    }
    else if (task) {
        if (message->getReply() == Error) {
            task->setError(message);
            mTask.clear();
            mWatchdog.stop();
            emit task->failed(task);
        } else {
            task->addReply(message);
            if (task->isComplete()) {
                mTask.clear();
                mWatchdog.stop();
                emit task->succeeded(task);
                sendFromQueue();
            } else {
                mWatchdog.start(); // restart
            }
        }
    }
    else if (message->getReply() == Error) {
        if (message->getError() == 4) {
            emit log("Firmwarefehler: queue overflow error: Lenlab liest die Daten vom Logger nicht ausreichend schnell");
        }
        else {
            emit log(QString("Firmwarefehler: Fehlercode %1").arg(message->getError()));
        }
    }
    else {
        emit log("Unerwartetes Paket empfangen");
    }
}

void
Board::on_error(const QString & message)
{
    auto device = mDevice;
    auto task = mTask;

    mPollTimer.stop();
    mBootTimer.stop();
    mWatchdog.stop();

    mTask.clear();
    mDevice.clear();

    if (task) {
        task->setError(message);
        emit task->failed(task);
    } else {
        emit error(message);
    }
}

void
Board::on_destroyed()
{
    emit log("Verbindung getrennt.");
    clearQueue();
    mPollTimer.start(mErrorTime);
}

void
Board::on_poll_timeout()
{
    lookForBoard(mBootTime);
}

void
Board::on_boot_timeout()
{
    pTask init(new Task(::init));
    connect(init.data(), &Task::succeeded,
            this, &Board::on_init);
    connect(init.data(), &Task::failed,
            this, &Board::on_task_error);
    startTask(init);
}

void
Board::on_watchdog_timeout()
{
    auto task = mTask;

    if (task) {
        task->setTimeoutError();
        mTask.clear();
        emit task->failed(task);
    }
}

void
Board::on_init(pTask const &)
{
    pTask name(new Task(::getName));
    connect(name.data(), &Task::succeeded,
            this, &Board::on_name);
    connect(name.data(), &Task::failed,
            this, &Board::on_task_error);
    startTask(name);
}

void
Board::on_name(pTask const &)
{
    pTask version(new Task(::getVersion));
    connect(version.data(), &Task::succeeded,
            this, &Board::on_version);
    connect(version.data(), &Task::failed,
            this, &Board::on_task_error);
    startTask(version);
}

void
Board::on_version(pTask const & task)
{
    auto device = mDevice;

    auto reply = task->getReply();
    auto length = reply->getUInt32BufferLength();
    if (length == 3 || length == 2) {
        auto array = reply->getUInt32Buffer();

        mMajor = array[0];
        mMinor = array[1];

        if (mMajor == MAJOR && mMinor == MINOR) {
            emit ready();
        }
        else {
            auto msg = QString("Ungültige Version %1.%2. Lenlab erwartet mindestens %3.%4.").arg(mMajor).arg(mMinor).arg(MAJOR).arg(MINOR);
            mDevice.clear();
            emit error(msg);
        }
    }
    else {
        auto msg = QString("Das Lenlab-Board antwortet mit einer ungültigen Version.");
        mDevice.clear();
        emit error(msg);
    }
}

void
Board::on_task_error(pTask const & task)
{
    auto device = mDevice;
    mDevice.clear();
    emit error(task->getErrorMessage());
}

/*
pTransaction
Board::init()
{
    auto transaction = pTransaction::create(::init);
    send(transaction);

    return transaction;
}

pTransaction
Board::getName()
{
    auto transaction = pTransaction::create(::getName);
    send(transaction);

    return transaction;
}

pTransaction
Board::getVersion()
{
    auto transaction = pTransaction::create(::getVersion);
    send(transaction);

    return transaction;
}

pTransaction
Board::setSignalSine(uint32_t multiplier, uint32_t predivider, uint32_t divider, uint32_t amplitude, uint32_t second)
{
    QVector<uint32_t> args;
    args.append(multiplier);
    args.append(predivider);
    args.append(divider);
    args.append(amplitude);
    args.append(second);

    auto transaction = pTransaction::create(::setSignalSine);
    transaction->command->setUInt32Vector(args);
    send(transaction);

    return transaction;
}

pTransaction
Board::startOscilloscope(uint32_t samplerate)
{
    QVector<uint32_t> args;
    args.append(samplerate);

    auto transaction = pTransaction::create(::startOscilloscope);
    transaction->command->setUInt32Vector(args);
    send(transaction);

    return transaction;
}

pTransaction
Board::startOscilloscopeLinearTestData()
{
    auto transaction = pTransaction::create(::startOscilloscopeLinearTestData);
    send(transaction);

    return transaction;
}

pTransaction
Board::startTriggerLinearTestData()
{
    auto transaction = pTransaction::create(::startTriggerLinearTestData);
    send(transaction);

    return transaction;
}

pTransaction
Board::startTrigger(uint32_t samplerate)
{
    QVector<uint32_t> args;
    args.append(samplerate);

    auto transaction = pTransaction::create(::startTrigger);
    transaction->command->setUInt32Vector(args);
    transaction->setWatchdog(800);
    send(transaction);

    return transaction;
}

pTransaction
Board::startLogger(uint32_t interval)
{
    QVector<uint32_t> args;
    args.append(interval);

    auto transaction = pTransaction::create(::startLogger);
    transaction->command->setUInt32Vector(args);
    send(transaction);

    return transaction;
}

pTransaction
Board::stopLogger()
{
    auto transaction = pTransaction::create(::stopLogger);
    send(transaction);

    return transaction;
}
*/
} // namespace protocol
