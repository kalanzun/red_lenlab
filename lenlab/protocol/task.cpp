#include "task.h"

namespace protocol {

static int p_task_type_id = qRegisterMetaType<pTask>("pTask");

Task::Task(Command code, int timeout, QObject * parent)
    : QObject(parent)
    , mCommand(new Message())
    , mTimeout(timeout)
{
    mCommand->setCommand(code);
}

pMessage const &
Task::getCommand() const
{
    return mCommand;
}

int
Task::getTimeout() const
{
    return mTimeout;
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
Task::setTimeoutError()
{
    mTimeoutError = true;
}

QString
Task::getErrorMessage() const
{
    if (mErrorMessage) {
        return QString("Firmwarefehler: Fehler in %1: Fehlercode %2.").arg(mCommand->getCommandName()).arg(mErrorMessage->getError());
    } else if (!mErrorString.isEmpty()) {
        return mErrorString;
    } else if (mTimeoutError) {
        return QString("Zeitüberschreitungsfehler: %1 wurde nicht innerhalb von %2 ms abgeschlossen.").arg(mCommand->getCommandName()).arg(mTimeout);
    } else {
        return QString();
    }
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
