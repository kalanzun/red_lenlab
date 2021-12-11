#ifndef PROTOCOL_QUERYTHREAD_H
#define PROTOCOL_QUERYTHREAD_H

#include <memory>

#include <QThread>

namespace protocol {

class Device;

class QueryThread : public QThread
{
    Q_OBJECT

    static const int interval = 300;

    uint16_t vid;
    uint16_t pid;

public:
    explicit QueryThread(uint16_t vid, uint16_t pid, QObject *parent = nullptr);
    ~QueryThread();

protected:
    void run() override;

signals:
    void DeviceCreated(protocol::Device* device);
    void Statistics(int count, int interval, int runtime);
};

} // namespace protocol

#endif // PROTOCOL_QUERYTHREAD_H
