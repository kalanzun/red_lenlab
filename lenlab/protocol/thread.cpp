#include "thread.h"

#include <QDebug>

#include "libusb.h"

namespace protocol {

Thread::Thread(QObject *parent)
    : QThread{parent}
{
    qDebug() << "Thread";
}

Thread::~Thread()
{
    qDebug() << "~Thread";
    wait();
}

void Thread::run()
{
    qDebug() << "thread starts";

    while(!isInterruptionRequested()) {
        libusb_handle_events(nullptr);
        qDebug() << "thread wakes up";
    }

    qDebug() << "thread stops";
}

} // namespace protocol
