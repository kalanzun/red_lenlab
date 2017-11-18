#ifndef DEVICE_H
#define DEVICE_H

#include "devhandle.h"
#include "eventloop.h"
#include "interface.h"
#include "message.h"
#include "thread.h"
#include "transfer.h"
#include "libusb.h"
#include <QObject>
#include <QSharedPointer>
#include <memory>

namespace usb {

    class Device : public QObject
    {
        Q_OBJECT

        std::unique_ptr<Thread> thread;
        resource::DevHandle dev_handle;
        resource::EventLoop event_loop;
        resource::Interface interface;
        QSharedPointer<QVector<pMessage>> send_queue;
        std::unique_ptr<Transfer> sender;
        std::unique_ptr<Transfer> receiver0;
        std::unique_ptr<Transfer> receiver1;

    public:
        explicit Device(libusb_device *dev, QObject *parent = nullptr);

        void send(const pMessage &cmd);

    signals:
        void reply(const pMessage &);
        void error(const QString &);

    private slots:
        void on_reply_transfer_ready();
        void on_send_transfer_ready();

    private:
        void try_to_send();

    };

}

#endif // DEVICE_H
