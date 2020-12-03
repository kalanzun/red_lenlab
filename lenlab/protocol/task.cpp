/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2020 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
