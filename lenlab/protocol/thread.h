#ifndef PROTOCOL_THREAD_H
#define PROTOCOL_THREAD_H

#include <QThread>

namespace protocol {

class Thread : public QThread
{
    Q_OBJECT

public:
    explicit Thread(QObject *parent = nullptr);

    ~Thread();

    void run() override;
};

} // namespace protocol

#endif // PROTOCOL_THREAD_H
