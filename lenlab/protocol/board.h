/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

#ifndef BOARD_H
#define BOARD_H

#include "message.h"
#include "task.h"

#include "usb/bus.h"
#include "usb/device.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTimer>

namespace protocol {

class Board : public QObject
{
    Q_OBJECT

    static int const mPollTime = 500;
    static int const mBootTime = 1200;
    static int const mErrorTime = 3000;
    static int const mWatchdogTime = 100;

    QTimer mPollTimer;
    QTimer mBootTimer;
    QTimer mWatchdog;

    QVector<pTask> mTaskQueue; // this one needs to stay alive until mDevice is destroyed

    usb::Bus mBus;
    usb::pDevice mDevice;
    pTask mTask;

    uint32_t mMajor, mMinor;

public:
    explicit Board(QObject * parent = nullptr);
    Board(Board const & other) = delete;

    Board & operator=(Board const & other) = delete;

    void lookForBoard(int boottime = 0);

    bool isOpen() const;
    bool isReady() const;
    void startTask(pTask const & task);
    void queueTask(pTask const & task);

    uint32_t getVersionMajor() const;
    uint32_t getVersionMinor() const;

    /*
    pTransaction init();
    pTransaction getName();
    pTransaction getVersion();
    pTransaction setSignalSine(uint32_t multiplier, uint32_t predivider, uint32_t divider, uint32_t amplitude, uint32_t second);
    pTransaction startOscilloscope(uint32_t samplerate);
    pTransaction startOscilloscopeLinearTestData();
    pTransaction startTrigger(uint32_t samplerate);
    pTransaction startTriggerLinearTestData();
    pTransaction startLogger(uint32_t interval);
    pTransaction stopLogger();
    */

private:
    void sendFromQueue();
    void clearQueue();

signals:
    void ready();
    void log(QString const &);
    void error(QString const &);

    void logger_data(pMessage const &);

private slots:
    void on_reply(usb::pPacket const &);
    void on_error(QString const &);
    void on_destroyed();

    void on_poll_timeout();
    void on_boot_timeout();
    void on_watchdog_timeout();

    void on_init(pTask const &);
    void on_name(pTask const &);
    void on_version(pTask const &);
    void on_task_error(pTask const &);
};

typedef QSharedPointer<Board> pBoard;

} // namespace protocol

#endif // BOARD_H
