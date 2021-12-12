#include "board.h"

#include <QDebug>

#include "usb/devicehandle.h"
#include "usb/usbexception.h"
#include "device.h"
#include "message.h"
#include "querythread.h"
#include "usbdevice.h"
#include "virtualdevice.h"

namespace protocol {

Board::Board(QObject *parent)
    : QObject{parent}
    , query_thread{std::make_unique< QueryThread >(LENLAB_VID, LENLAB_PID)}
{
#ifndef NDEBUG
    query_thread->retry = false;
#endif

    connect(query_thread.get(), &QueryThread::DeviceHandleCreated,
            this, &Board::handleDeviceHandleCreated);

    connect(query_thread.get(), &QueryThread::Statistics,
            this, &Board::handleQueryThreadStatistics);
}

// TODO rename to send and receive (board->command() might create or send a command)
void Board::command(std::shared_ptr< Message >& message)
{
    assert(device);

    if (device) device->send(message);
}

void Board::lookForDevice(bool create_virtual_device)
{
    assert(!query_thread->isRunning());

    query_thread->start();
}

void Board::setupDevice()
{
    assert(device);

    connect(device.get(), &Device::reply, this, &Board::handleReply);
    connect(device.get(), &Device::error, this, &Board::handleError);
    connect(device.get(), &Device::destroyed, this, &Board::lookForDevice);

    auto setup = Message::createCommand(setUp);
    command(setup);
}

void Board::handleDeviceHandleCreated(std::shared_ptr< usb::DeviceHandle > device_handle)
{
    assert(!device);

    device = std::make_shared< USBDevice >(device_handle);
    setupDevice();
}

void Board::handleQueryThreadStatistics(int count, int interval, int runtime)
{
    qDebug() << "QueryThreadStatistics" << count << interval << runtime;

#ifndef NDEBUG
    query_thread->wait(); // it did not retry

    assert(!device);
    device = std::make_shared< VirtualDevice >();
    setupDevice();
#endif
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

    device.reset();

    emit error();
}

} // namespace protocol
