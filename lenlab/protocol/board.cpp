#include "board.h"

#include "usb/usberror.h"

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
    if (mDevice) throw AlreadyConnected();

    try {
        mDevice = mBus.query(LENLAB_VID, LENLAB_PID);
        connect(mDevice.get(), &usb::Device::reply,
                this, &Board::on_reply);
        connect(mDevice.get(), &usb::Device::error,
                this, &Board::on_error);
        connect(mDevice.get(), &usb::Device::destroyed,
                this, &Board::on_destroyed);
        emit log("Lenlab-Board gefunden.");
        // wait for the board to boot, it just got power
        mBootTimer.start(boottime);
    } catch (usb::NotFound const & e) {
        mPollTimer.start(mPollTime);
    } catch (usb::UsbErrorMessage const & e) {
        emit error(e.getMsg());
        mPollTimer.start(mErrorTime);
    }
}

bool
Board::isOpen() const
{
    return mDevice;
}

bool
Board::isReady() const
{
    // TODO rename ready signal open, maybe implement closed signal
    return mDevice && !mTask;
}

pTask const &
Board::startTask(pMessage const & command, int timeout)
{
    if (!mDevice) throw NotConnected();
    if (mTask) throw Busy();

    mTask.reset(new Task(command, timeout));

    mDevice->send(command->getPacket());
    mWatchdog.start(timeout);

    return mTask;
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
Board::on_reply(usb::pPacket const & packet)
{
    auto task = mTask;
    auto message = pMessage::create(packet);

    if (task) {
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
            } else {
                mWatchdog.start(); // restart
            }
        }
    } else if (message->getReply() == LoggerData) {
        emit logger_data(message);
    } else {
        emit log("Unerwartetes Paket empfangen.");
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
    pMessage cmd(new Message());
    cmd->setCommand(::init);
    auto init = startTask(cmd);
    connect(init.data(), &Task::succeeded,
            this, &Board::on_init);
    connect(init.data(), &Task::failed,
            this, &Board::on_task_error);
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
Board::on_init(pTask const & task)
{
    pMessage cmd(new Message());
    cmd->setCommand(::getName);
    auto name = startTask(cmd);
    connect(name.data(), &Task::succeeded,
            this, &Board::on_name);
    connect(name.data(), &Task::failed,
            this, &Board::on_task_error);
}

void
Board::on_name(pTask const & task)
{
    pMessage cmd(new Message());
    cmd->setCommand(::getVersion);
    auto version = startTask(cmd);
    connect(version.data(), &Task::succeeded,
            this, &Board::on_version);
    connect(version.data(), &Task::failed,
            this, &Board::on_task_error);
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
