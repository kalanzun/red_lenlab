#include "board.h"

#include <QDebug>

#include "usb/devicehandle.h"
#include "usb/usbexception.h"
#include "device.h"
#include "message.h"
#include "querythread.h"

namespace protocol {

Board::Board(QObject *parent)
    : QObject{parent}
    , query_thread{std::make_unique< QueryThread >(LENLAB_VID, LENLAB_PID)}
{
    connect(query_thread.get(), &QueryThread::DeviceCreated,
            this, &Board::setupDevice);

    connect(query_thread.get(), &QueryThread::Statistics,
            this, &Board::handleQueryThreadStatistics);
}

// TODO rename to send and receive (board->command() might create or send a command)
void Board::command(std::shared_ptr< Message >& message)
{
    assert(device);

    if (device) device->send(message);
}

void Board::lookForDevice()
{
    assert(!query_thread->isRunning());

    query_thread->start();
}

void Board::setupDevice(protocol::Device* device)
{
    assert(!device);

    this->device = device;
    connect(device, &Device::reply, this, &Board::handleReply);
    connect(device, &Device::error, this, &Board::handleError);
    connect(device, &Device::destroyed, this, &Board::lookForDevice);

    auto setup = Message::createCommand(setUp);
    command(setup);
}

void Board::handleQueryThreadStatistics(int count, int interval, int runtime)
{
    qDebug() << "QueryThreadStatistics" << count << interval << runtime;
}

void Board::handleReply(std::shared_ptr< Message >& message)
{
    if (message->head->reply == Setup) {
        emit setup(message);
    }
    else {
        emit reply(message);
    }
}

void Board::handleError()
{
    assert(device);

    device->deleteLater();

    emit error();
}

} // namespace protocol
