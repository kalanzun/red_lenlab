#include "device.h"
#include "exception.h"
#include <QDebug>

using namespace usb;

Device::Device(libusb_device *dev, QObject *parent) :
    QObject(parent),
    thread(new Thread()),
    dev_handle(dev),
    event_loop(thread.get()),
    interface(dev_handle.get()),
    send_queue(new QVector<pMessage>()),
    sender(new Transfer(dev_handle.get(), 0x01)),
    receiver0(new Transfer(dev_handle.get(), 0x81)),
    receiver1(new Transfer(dev_handle.get(), 0x81))
{
    connect(&*receiver0, SIGNAL(completed(pMessage)),
            this, SIGNAL(reply(pMessage)));
    connect(&*receiver1, SIGNAL(completed(pMessage)),
            this, SIGNAL(reply(pMessage)));

    connect(&*sender, SIGNAL(completed(pMessage)),
            this, SLOT(on_send_transfer_ready()));
    connect(&*receiver0, SIGNAL(completed(pMessage)),
            this, SLOT(on_reply_transfer_ready()));
    connect(&*receiver1, SIGNAL(completed(pMessage)),
            this, SLOT(on_reply_transfer_ready()));

    receiver0->start(pMessage(new Message()));
    receiver1->start(pMessage(new Message()));

    thread->start();
}

void
Device::send(const pMessage &cmd)
{
    send_queue->append(cmd);
    try_to_send();
}

void
Device::on_reply_transfer_ready()
{
    qobject_cast<Transfer *>(QObject::sender())->start(pMessage(new Message()));
}

void
Device::try_to_send()
{
    if (!send_queue->isEmpty() && !sender->isActive()) {
        sender->start(send_queue->takeFirst());
    }
}

void
Device::on_send_transfer_ready()
{
    try_to_send();
}
