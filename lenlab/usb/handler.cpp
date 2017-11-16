#include "handler.h"
#include "devicelist.h"
#include "exception.h"

using namespace usb;

Handler::Handler(QObject *parent) : QObject(parent)
{
    qInfo("Handler constructor");

    timer_id = startTimer(250);
}

bool
Handler::isReady()
{
    return true;
}

void Handler::queryUSBDevices() {
    struct libusb_device_descriptor desc;
    resource::DeviceList device_list;

    for(auto dev: device_list)
    {
       libusb_get_device_descriptor(dev, &desc);

       // Is it our device?
       if(desc.idVendor == 0x1cbe && desc.idProduct == 0x0003)
       {
          // Open USB device
          device.reset(new Device(dev));
          connect(device.data(), SIGNAL(reply(pMessage)), this, SIGNAL(reply(pMessage)));
          connect(this, SIGNAL(command(pMessage)), device.data(), SIGNAL(command(pMessage)));

          break;
       }
    }
}

void Handler::timerEvent(QTimerEvent *event)
{
    if (!device) {
        queryUSBDevices();
    }
    else {
        emit ready();
        killTimer(timer_id);
    }
}
