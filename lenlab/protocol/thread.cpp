#include "thread.h"

#include <QDebug>

#include "libusb.h"

namespace protocol {

Thread::Thread(std::shared_ptr< usb::Device >& device, QObject* parent)
    : QThread(parent)
    , m_device(device)
{

}

void Thread::receiveCallback(void* object, std::shared_ptr< usb::Packet >& packet)
{
    auto thread = static_cast< Thread* >(object);
    qDebug() << "Thread::receiveCallback";
    emit thread->receive(packet);
}

void Thread::run()
{
    m_device->m_object = this;
    m_device->m_receive_callback = receiveCallback;

    while(!isInterruptionRequested()) {
        qDebug() << "ping";
        libusb_handle_events(nullptr);
    }
}

void Thread::transmit(std::shared_ptr< usb::Packet >& packet)
{
    qDebug() << "Thread::transmit";
    m_device->transmit(packet);
}

} // namespace protocol
