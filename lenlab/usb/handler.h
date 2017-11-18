#ifndef USBHANDLER_H
#define USBHANDLER_H

#include "context.h"
#include "message.h"
#include "device.h"
#include <QObject>
#include <QSharedPointer>

namespace usb {

    class Handler : public QObject
    {
        Q_OBJECT

        resource::Context ctx;
        QSharedPointer<Device> device;
        int timer_id;

    public:
        explicit Handler(QObject *parent = nullptr);

        void send(const pMessage &cmd);

        bool isReady();

    signals:
        void reply(const pMessage &);
        void ready();
        void logMessage(const QString &);

        void query();

    private slots:
        void on_error(const QString &);
        void on_query();

    private:
        void open(libusb_device *dev);

        void timerEvent(QTimerEvent *event);
    };

}

#endif // USBHANDLER_H
