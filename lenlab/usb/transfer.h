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

#ifndef USBTRANSFER_H
#define USBTRANSFER_H

#include "active.h"
#include "packet.h"
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
        ~Transfer() override;

        bool isActive();

    signals:
        void completed(const pPacket &);
        void error(const QString &);

    public slots:
        void start(const pPacket &);

    private:
        static void LIBUSB_CALL callbackComplete(struct libusb_transfer *xfr);
    };

}

#endif // USBTRANSFER_H
