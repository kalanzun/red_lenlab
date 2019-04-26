#include "board.h"

#include <QMetaMethod>

namespace protocol {

static int p_board_type_id = qRegisterMetaType<pBoard>("pBoard");

Board::Board(usb::pDevice &device, QObject *parent)
    : QObject(parent)
    , mDevice(device)
{
    connect(device.data(), &usb::Device::reply,
            this, &Board::on_reply);

    connect(device.data(), &usb::Device::error,
            this, &Board::on_error);

    connect(&mWatchdog, &QTimer::timeout,
            this, &Board::on_timeout);

    mWatchdog.setInterval(100);
    mWatchdog.setSingleShot(true);
}


pTask const &
Board::startTask(pMessage const & command, int timeout)
{
    Q_ASSERT(!mTask);

    mTask.reset(new Task(command, timeout));

    mDevice->send(command->getPacket());
    mWatchdog.start(timeout);

    return mTask;
}

void
Board::setVersion(uint32_t major, uint32_t minor)
{
    mMajor = major;
    mMinor = minor;
}

uint32_t
Board::getMajor() const
{
    return mMajor;
}

uint32_t
Board::getMinor() const
{
    return mMinor;
}

void
Board::on_reply(usb::pPacket const & packet)
{
    auto message = pMessage::create(packet);
    auto task = mTask;

    if (task) {
        mWatchdog.start(); // restart
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
            }
        }
    } else if (message->getReply() == LoggerData) {
        emit logger(message);
    } else {
        emit error("Unerwartetes Paket empfangen.");
    }
}

void
Board::on_error(const QString & message)
{
    auto task = mTask;

    if (task) {
        task->setError(message);
        mTask.clear();
        emit task->failed(task);
    } else {
        emit error(message);
    }
}


void
Board::on_timeout()
{
    auto task = mTask;

    if (task) {
        task->setTimeoutError();
        mTask.clear();
        emit task->failed(task);
    }
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
