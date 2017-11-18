#ifndef LENLAB_H
#define LENLAB_H

#include "frequency.h"
#include "logger.h"
#include "oscilloscope.h"
#include "signal.h"
#include "usb/handler.h"
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
    virtual ~Lenlab();

    void setHandler(usb::Handler *handler);

    bool isActive();
    Component *getActiveComponent();

    void send(const usb::pMessage &cmd);

    Frequency *frequency;
    Logger *logger;
    Oscilloscope *oscilloscope;
    Signal *signal;

signals:
    void reply(const usb::pMessage &);

    void logMessage(const QString &);

    void replot();

public slots:

private slots:
    void on_reply(const usb::pMessage &);
    void on_ready();
    void on_error();

private:
    usb::Handler *handler;

    bool ignore = true;
};

} // namespace model

#endif // LENLAB_H
