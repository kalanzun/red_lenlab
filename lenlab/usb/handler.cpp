#include "handler.h"
#include "devicelist.h"
#include "exception.h"
#include <QTimerEvent>
#include <QDebug>

using namespace usb;

Handler::Handler(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(query()),
            this, SLOT(on_query()),
            Qt::QueuedConnection);
    emit query();
}

void
Handler::send(const pMessage &cmd)
{
    if (device) {
        device->send(cmd);
    }
}

bool
Handler::isReady()
{
    return true;
}

void
Handler::on_query() {
    struct libusb_device_descriptor desc;
    resource::DeviceList device_list;

    for(auto dev: device_list) {
        libusb_get_device_descriptor(dev, &desc);

        // Is it our device?
        if(desc.idVendor == 0x1cbe && desc.idProduct == 0x0003) {
            open(dev);
            return;
        }
    }

    startTimer(500); // fast polling when no device found
}

void
Handler::open(libusb_device *dev)
{
    try {
        device.reset(new Device(dev));
        connect(device.data(), SIGNAL(reply(pMessage)),
                this, SIGNAL(reply(pMessage)));
        connect(device.data(), SIGNAL(error(QString)),
                this, SLOT(on_error(QString)));
        emit logMessage("USB: Lenlab board verbunden.");
        emit ready();
    }
    catch (Exception e) {
        startTimer(2000); // slow polling due to error
        emit logMessage(QString("USB: Fehler beim Verbindungsaufbau: %1.").arg(e.getMsg()));
    }
}

void
Handler::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    if (!device) {
        on_query();
    }
}

void
Handler::on_error(const QString &msg)
{
    emit logMessage(QString("USB: Fehler bei der Kommunikation: %1.").arg(msg));

    device.clear();
    startTimer(2000); // slow polling to try again
}
