#ifndef PROTOCOL_USBTHREAD_H
#define PROTOCOL_USBTHREAD_H

#include <QThread>

namespace protocol {

class USBThread : public QThread
{
    Q_OBJECT

public:
    explicit USBThread(QObject *parent = nullptr);
    ~USBThread();

    void run() override;
};

} // namespace protocol

#endif // PROTOCOL_USBTHREAD_H
