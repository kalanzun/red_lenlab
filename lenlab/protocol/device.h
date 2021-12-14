#ifndef PROTOCOL_DEVICE_H
#define PROTOCOL_DEVICE_H

#include <memory>

#include <QObject>

namespace protocol {

class Message;

class Device : public QObject
{
    Q_OBJECT

public:
    explicit Device(QObject *parent = nullptr);

signals:
    void reply(const std::shared_ptr< Message >& message);
    void error();

public slots:
    virtual void send(const std::shared_ptr< Message >& message) = 0;
};

} // namespace protocol

#endif // PROTOCOL_DEVICE_H
