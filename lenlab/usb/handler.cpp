/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "handler.h"
#include "devicelist.h"
#include "exception.h"
#include "lenlab_protocol.h"
#include "config.h"
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
        if(desc.idVendor == LENLAB_VID && desc.idProduct == LENLAB_PID) {
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
                this, SLOT(on_reply(pMessage)));
        connect(device.data(), SIGNAL(error(QString)),
                this, SLOT(on_error(QString)));

        emit logMessage("Lenlab-Board gefunden.");

        //emit logMessage("Initialisierung");
        usb::pMessage cmd(new usb::Message());
        cmd->setCommand(init);
        send(cmd);
    }
    catch (Exception e) {
        startTimer(2000); // slow polling due to error
        emit logMessage(QString("Fehler beim Verbindungsaufbau: %1.").arg(e.getMsg()));
    }
}

void
Handler::on_reply(const pMessage &reply)
{
    if (reply->getCommand() == init) {
        emit logMessage("Initialisierung abgeschlossen.");

        //emit logMessage("Firmware-Name erfragen");
        usb::pMessage cmd(new usb::Message());
        cmd->setCommand(getName);
        send(cmd);
    }
    else if (reply->getCommand() == getName) {
        QString name((char *) reply->getPayload());//, reply->getPayloadLength());
        //emit logMessage(QString("Firmware-Name: %1.").arg(name));

        //emit logMessage("Firmware-Version erfragen");
        usb::pMessage cmd(new usb::Message());
        cmd->setCommand(getVersion);
        send(cmd);
    }
    else if (reply->getCommand() == getVersion) {
        uint32_t *version = (uint32_t *) reply->getPayload();
        emit logMessage(QString("Firmware-Version %1.%2.%3.").arg(version[0]).arg(version[1]).arg(version[2]));
        //emit logMessage(QString("Lenlab-Version: " STR(MAJOR) "." STR(MINOR) "." STR(REVISION) "."));

        if (reply->getPayloadLength() == 12 && version[0] == MAJOR && version[1] == MINOR) {
            disconnect(device.data(), SIGNAL(reply(pMessage)),
                    this, SLOT(on_reply(pMessage)));
            connect(device.data(), SIGNAL(reply(pMessage)),
                    this, SIGNAL(reply(pMessage)));
            emit logMessage("Lenlab-Board bereit.");
            emit ready();
        }
        else {
            emit logMessage("Ungeeignete Firmware-Version. Bitte verwenden Sie die Firmware-Version " STR(MAJOR) "." STR(MINOR) " für dieses Lenlab.");

            device.clear();
            startTimer(2000); // slow polling to try again
        }
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
