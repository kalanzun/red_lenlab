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
    connect(query_thread.get(), &QueryThread::DeviceHandleCreated,
            this, &Board::handleDeviceHandleCreated);

    connect(query_thread.get(), &QueryThread::Statistics,
            this, &Board::handleQueryThreadStatistics);
}

void Board::lookForDevice(bool create_virtual_device)
{
    assert(!query_thread->isRunning());

    if (create_virtual_device) {
        assert(!device);
        device = std::make_shared< VirtualDevice >();
        setupDevice();
    }
    else {
        query_thread->start();
    }
}

void Board::send(std::shared_ptr< Message >& message)
{
    assert(device);

    if (device) device->send(message);
}

void Board::setupDevice()
{
    assert(device);

    connect(device.get(), &Device::reply, this, &Board::handleReply);
    connect(device.get(), &Device::error, this, &Board::handleError);
    connect(device.get(), &Device::destroyed, this, &Board::lookForDevice);

    auto setup = Message::createCommand(setUp);
    send(setup);
}

void Board::handleDeviceHandleCreated(std::shared_ptr< usb::DeviceHandle > device_handle)
{
    assert(!device);

    device = std::make_shared< USBDevice >(device_handle);
    setupDevice();
}

void Board::handleQueryThreadStatistics(int count, int interval, int runtime)
{
    query_thread_statistics.count = count;
    query_thread_statistics.interval = interval;
    query_thread_statistics.runtime = runtime;
    if (runtime > query_thread_statistics.runtime_max) query_thread_statistics.runtime_max = runtime;
    if (runtime < query_thread_statistics.runtime_min) query_thread_statistics.runtime_min = runtime;
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
