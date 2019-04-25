#ifndef TASK_H
#define TASK_H

#include "message.h"

#include <QObject>
#include <QSharedPointer>
#include <QVector>

namespace protocol {

class Task;

typedef QSharedPointer<Task> pTask;

class Task : public QObject
{
    Q_OBJECT

    QVector<pMessage> mReplies;
    bool mComplete = false;
    pMessage mErrorMessage;
    QString mErrorString;
    bool mTimeout = false;

public:
    explicit Task(QObject *parent = nullptr);
    Task(Task const & other) = delete;

    Task & operator=(Task const & other) = delete;

    void addReply(pMessage const &);
    bool isComplete() const;
    void setError(pMessage const &);
    void setError(QString const &);
    void setTimeout();
    bool isTimeout() const;

    pMessage const & getReply() const;
    int getSize() const;

signals:
    void succeeded(pTask const &);
    void failed(pTask const &);
};

} // namespace protocol

#endif // TASK_H
