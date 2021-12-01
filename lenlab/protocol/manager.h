#ifndef PROTOCOL_MANAGER_H
#define PROTOCOL_MANAGER_H

#include <memory>

#include <QObject>
#include <QTimer>

#include "usb/context.h"

#include "device.h"

namespace protocol {

class Manager : public QObject
{
    Q_OBJECT

    static int const kPollTime = 300;
    static int const kRetryTime = 1000;

    usb::Context mContext;

    std::shared_ptr< Device > mDevice;

    QTimer mPollTimer;

public:
    explicit Manager(QObject *parent = nullptr);

signals:

public slots:
    void lookForBoard();

private slots:
    void poll();
    void resetDevice();

};

} // namespace protocol

#endif // PROTOCOL_MANAGER_H
