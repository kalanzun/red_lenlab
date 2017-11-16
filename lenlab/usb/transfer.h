#ifndef USBTRANSFER_H
#define USBTRANSFER_H

#include "active.h"
#include "message.h"
#include "libusb.h"
#include <QObject>
#include <functional>
#include <memory>

namespace usb {

    class Transfer : public QObject
    {
        Q_OBJECT

        Active active;
        std::unique_ptr<struct libusb_transfer, std::function<void(struct libusb_transfer *)>> xfr;

    public:
        explicit Transfer(libusb_device_handle *dev_handle, unsigned char endpoint, QObject *parent = nullptr);
        virtual ~Transfer();

        bool isActive();

    signals:
        void completed(const pMessage&);
        void error();

    public slots:
        void start(const pMessage&);

    private:
        static void LIBUSB_CALL callbackComplete(struct libusb_transfer *xfr);
    };

}

#endif // USBTRANSFER_H
