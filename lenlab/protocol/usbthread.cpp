#include "usbthread.h"

#include "libusb.h"

namespace protocol {

USBThread::USBThread(QObject *parent)
    : QThread{parent}
{

}

USBThread::~USBThread()
{
    wait();
}

void USBThread::run()
{
    while(!isInterruptionRequested()) {
        libusb_handle_events(nullptr);
    }
}

} // namespace protocol
