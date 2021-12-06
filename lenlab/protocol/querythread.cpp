#include "querythread.h"

#include <chrono>

#include <QDebug>

#include "libusb.h"

#include "usb/devicelist.h"

namespace protocol {

QueryThread::QueryThread(uint16_t vid, uint16_t pid, QObject *parent)
    : QThread{parent}
    , vid{vid}
    , pid{pid}
{

}

QueryThread::~QueryThread()
{
    requestInterruption();
    wait();
}

void QueryThread::run()
{
    struct libusb_device_descriptor desc;

    while(!isInterruptionRequested()) {
        auto start = std::chrono::steady_clock::now();

        usb::DeviceList list; // slow and may throw

        for(auto dev: list) {
            libusb_get_device_descriptor(dev, &desc);

            if(desc.idVendor == vid && desc.idProduct == pid) {
                auto device_handle = std::make_shared< usb::DeviceHandle >(dev); // may throw
                emit DeviceHandleCreated(device_handle);
                return;
            }
        }

        auto interval = std::chrono::duration_cast< std::chrono::milliseconds >(
                    std::chrono::steady_clock::now() - start);
        qDebug() << "QueryThread loop interval [ms]:" << interval.count();
    }
}

} // namespace protocol
