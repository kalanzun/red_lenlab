#ifndef BOARD_H
#define BOARD_H

#include "message.h"
#include "task.h"

#include "usb/device.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTimer>

namespace protocol {

class Board : public QObject
{
    Q_OBJECT

    QTimer mWatchdog;
    usb::pDevice mDevice;
    pTask mTask;

public:
    explicit Board(usb::pDevice &device, QObject *parent = nullptr);
    Board(Board const & other) = delete;

    Board & operator=(Board const & other) = delete;

    const pTask & startTask(pMessage const & command, int timeout = 100);
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
    void logger(pMessage const &);
    void error(QString const &);

private slots:
    void on_reply(usb::pPacket const &);
    void on_error(QString const &);
    void on_timeout();
};

typedef QSharedPointer<Board> pBoard;

} // namespace protocol

#endif // BOARD_H
