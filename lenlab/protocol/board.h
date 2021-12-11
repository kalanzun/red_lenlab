#ifndef PROTOCOL_BOARD_H
#define PROTOCOL_BOARD_H

#include <memory>

#include <QObject>
#include <QPointer>

namespace protocol {

class Device;
class Message;
class QueryThread;

class Board : public QObject
{
    Q_OBJECT

    std::unique_ptr< QueryThread > query_thread;
    QPointer< Device > device;

public:
    explicit Board(QObject *parent = nullptr);

    void command(std::shared_ptr< Message >& message);

signals:
    void setup(std::shared_ptr< Message >& message);
    void reply(std::shared_ptr< Message >& message);
    void error();

public slots:
    void lookForDevice();

private slots:
    void setupDevice(protocol::Device* device);
    void handleQueryThreadStatistics(int count, int interval, int runtime);
    void handleReply(std::shared_ptr< Message >& message);
    void handleError();
};

} // namespace protocol

#endif // PROTOCOL_BOARD_H
