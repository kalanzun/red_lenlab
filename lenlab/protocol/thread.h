#ifndef PROTOCOL_THREAD_H
#define PROTOCOL_THREAD_H

#include <memory>

#include <QThread>

#include "usb/device.h"
#include "usb/packet.h"

namespace protocol {

class Thread : public QThread
{
    Q_OBJECT

    std::shared_ptr< usb::Device > m_device;

    static void receiveCallback(void* object, std::shared_ptr< usb::Packet >& packet);

public:
    explicit Thread(std::shared_ptr< usb::Device >& device, QObject *parent = nullptr);

    void run() override;

signals:
    void receive(std::shared_ptr< usb::Packet >& packet);

public slots:
    void transmit(std::shared_ptr< usb::Packet >& packet);

};

} // namespace protocol

#endif // PROTOCOL_THREAD_H
