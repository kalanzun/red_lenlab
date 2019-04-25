#include "task.h"

namespace protocol {

static int p_task_type_id = qRegisterMetaType<pTask>("pTask");

Task::Task(QObject *parent)
    : QObject(parent)
{

}

void
Task::addReply(const pMessage & message)
{
    mReplies.append(message);
    if (message->isLast()) {
        mComplete = true;
    }
}

bool
Task::isComplete() const
{
    return mComplete;
}

void
Task::setError(const pMessage & message)
{
    mErrorMessage = message;
}

void
Task::setError(const QString & error)
{
    mErrorString = error;
}

void
Task::setTimeout()
{
    mTimeout = true;
}

bool Task::isTimeout() const
{
    return mTimeout;
}

const pMessage &Task::getReply() const
{
    return mReplies.last();
}

int Task::getSize() const
{
    return mReplies.size();
}

const QVector<pMessage> &Task::getReplies() const
{
    return mReplies;
}

} // namespace protocol
