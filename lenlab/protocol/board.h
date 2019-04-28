#ifndef BOARD_H
#define BOARD_H

#include "message.h"
#include "protocolerror.h"
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
    static int const mTaskTime = 100;

    QTimer mPollTimer;
    QTimer mBootTimer;
    QTimer mWatchdog;

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
    const pTask & startTask(pMessage const & command, int timeout = mTaskTime);

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
