#ifndef LENLAB_H
#define LENLAB_H

#include "frequency.h"
#include "logger.h"
#include "oscilloscope.h"
#include "signal.h"
#include "usb/message.h"
#include <QObject>

namespace model {

/**
 * @brief Root object of the Lenlab data model.
 */

class Lenlab : public QObject
{
    Q_OBJECT
public:
    explicit Lenlab(QObject *parent = nullptr);

    bool isActive();
    Component *getActiveComponent();

    void send(const usb::pMessage &cmd);

    Frequency *frequency;
    Logger *logger;
    Oscilloscope *oscilloscope;
    Signal *signal;

signals:
    void command(const usb::pMessage &command);
    void reply(const usb::pMessage &reply);

public slots:

private slots:
    void on_reply(const usb::pMessage &reply);

private:
};

} // namespace model

#endif // LENLAB_H
