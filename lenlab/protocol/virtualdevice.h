#ifndef PROTOCOL_VIRTUALDEVICE_H
#define PROTOCOL_VIRTUALDEVICE_H

#include <QTimer>

#include "device.h"

namespace protocol {

class VirtualDevice : public Device
{
    Q_OBJECT

    static const float pi;
    static const float f;

    QTimer logger;
    int count = 0;

public:
    explicit VirtualDevice(QObject *parent = nullptr);

signals:
    void command(const std::shared_ptr< Message >& message);

public slots:
    void send(const std::shared_ptr< Message >& message);

private slots:
    void handleCommand(const std::shared_ptr< Message >& cmd);
    void logger_main();

private:
    void set_up(const std::shared_ptr< Message >& cmd);
    void start_logger(const std::shared_ptr< Message >& cmd);
    void stop_logger(const std::shared_ptr< Message >& cmd);
    void start_oscilloscope(const std::shared_ptr< Message >& cmd);
};

} // namespace protocol

#endif // PROTOCOL_VIRTUALDEVICE_H
