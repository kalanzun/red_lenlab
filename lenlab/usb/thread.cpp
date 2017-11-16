#include "thread.h"
#include "libusb.h"
#include <QDebug>

using namespace usb;

Thread::Thread(QObject *parent) : QThread(parent)
{
    qDebug("thread");
}

Thread::~Thread()
{
    qDebug("~thread");
    wait();
}

void
Thread::run()
{
    qDebug("thread start");
    while(!isInterruptionRequested()) {
        libusb_handle_events(nullptr);
    }
    qDebug("thread stop");
}
