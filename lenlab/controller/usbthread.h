#ifndef CONTROLLER_USBTHREAD_H
#define CONTROLLER_USBTHREAD_H

#include <QThread>

namespace controller {

class USBThread : public QThread
{
    Q_OBJECT

public:
    explicit USBThread(QObject *parent = nullptr);
    ~USBThread();

    void run() override;
};

} // namespace controller

#endif // CONTROLLER_USBTHREAD_H
