/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
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

    static int const mTaskTime = 100;

    pMessage mCommand;
    int mTimeout;

    QVector<pMessage> mReplies;
    bool mComplete = false;

    pMessage mErrorMessage;
    QString mErrorString;
    bool mTimeoutError = false;

public:
    explicit Task(Command code, int timeout = mTaskTime, QObject *parent = nullptr);
    Task(Task const & other) = delete;

    Task & operator=(Task const & other) = delete;

    pMessage const & getCommand() const;
    int getTimeout() const;

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
