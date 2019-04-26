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

    pMessage mCommand;
    int mTimeout;

    QVector<pMessage> mReplies;
    bool mComplete = false;

    pMessage mErrorMessage;
    QString mErrorString;
    bool mTimeoutError = false;


public:
    explicit Task(pMessage const & command, int timeout, QObject *parent = nullptr);
    Task(Task const & other) = delete;

    Task & operator=(Task const & other) = delete;

    void addReply(pMessage const &);
    bool isComplete() const;

    void setError(pMessage const &);
    void setError(QString const &);
    void setTimeoutError();

    QString getErrorMessage() const;

    pMessage const & getReply() const;
    int getSize() const;
    QVector<pMessage> const & getReplies() const;

signals:
    void succeeded(pTask const &);
    void failed(pTask const &);
};

} // namespace protocol

#endif // TASK_H
