#include "manager.h"

#include "lenlab_protocol.h"

#include "usb/usbexception.h"

namespace protocol {

Manager::Manager(QObject *parent)
    : QObject{parent}
{
    connect(&mPollTimer, &QTimer::timeout,
            this, &Manager::poll);

    mPollTimer.setSingleShot(true);
}

void Manager::lookForBoard()
{
    mPollTimer.start(kPollTime);
}

void Manager::poll()
{
    try {
        auto device_handle = usb::DeviceHandle::query(LENLAB_VID, LENLAB_PID);
        if (device_handle) {
            qDebug() << "board connected";

            mDevice = std::make_shared< Device >(std::move(device_handle));
            connect(mDevice.get(), &Device::reset, this, &Manager::resetDevice);
            //connect(thread, &Device::finished, thread, &QObject::deleteLater);
            //connect(this, &Board::send, device, &Device::send);

            auto packet = std::make_shared< usb::Packet >();
            packet->buffer[0] = setUp;
            packet->buffer[1] = nullType;
            packet->buffer[2] = 0;
            packet->buffer[3] = 0;
            packet->length = 4;
            mDevice->send(std::move(packet));
        }
        else {
            mPollTimer.start(kPollTime);
        }
    } catch (usb::USBException const &exception) {
        qDebug() << exception.message;
        mPollTimer.start(kRetryTime);
    }
}

void Manager::resetDevice()
{
    mDevice = nullptr;
    mPollTimer.start(kRetryTime);
}

} // namespace protocol
