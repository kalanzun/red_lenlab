#include "board.h"

#include <QDebug>

#include "lenlab_protocol.h"

#include "usb/devicehandle.h"
#include "usb/usbexception.h"

#include "thread.h"

namespace protocol {

Board::Board(QObject *parent)
    : QObject{parent}
{
    connect(&mPollTimer, &QTimer::timeout,
            this, &Board::poll);

    mPollTimer.setSingleShot(true);
}

void Board::lookForBoard()
{
    mPollTimer.start(mPollTime);
}

void Board::poll()
{
    try {
        auto device = usb::Device::query(LENLAB_VID, LENLAB_PID);
        if (device) {
            qDebug() << "connected";

            auto thread = new Thread(device, this);
            connect(thread, &Thread::receive, this, &Board::receive);
            connect(thread, &Thread::finished, thread, &QObject::deleteLater);
            connect(this, &Board::transmit, thread, &Thread::transmit);
            thread->start();

            auto packet = std::make_shared< usb::Packet >();
            packet->buffer[0] = setUp;
            packet->buffer[1] = nullType;
            packet->buffer[2] = 0;
            packet->buffer[3] = 0;
            packet->length = 64;
            emit transmit(packet);
        }
        else {
            mPollTimer.start(mPollTime);
        }
    } catch (usb::USBException const &exception) {
        qDebug() << exception.message();
        mPollTimer.start(mRetryTime);
    }
}

void Board::receive(std::shared_ptr< usb::Packet >& packet)
{
    qDebug() << "packet received";
}

} // namespace protocol
