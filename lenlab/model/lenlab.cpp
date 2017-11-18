#include "lenlab.h"
#include <QDebug>

namespace model {

Lenlab::Lenlab(QObject *parent) :
    QObject(parent),
    frequency(new Frequency(this)),
    logger(new Logger(this)),
    oscilloscope(new Oscilloscope(this)),
    signal(new Signal(this))
{
    qDebug() << "Lenlab";
    /*
    connect(this, SIGNAL(reply(usb::pMessage)),
            this, SLOT(on_reply(usb::pMessage)));
            */
}

Lenlab::~Lenlab()
{
    qDebug() << "~Lenlab";
}

void
Lenlab::setHandler(usb::Handler *handler)
{
    this->handler = handler;

    connect(handler, SIGNAL(reply(usb::pMessage)),
            this, SLOT(on_reply(usb::pMessage)));
    connect(handler, SIGNAL(ready()),
            this, SLOT(on_ready()));
    connect(handler, SIGNAL(error()),
            this, SLOT(on_error()));
}

bool
Lenlab::isActive()
{
    return frequency->isActive() || logger->isActive() || oscilloscope->isActive();
}

Component *
Lenlab::getActiveComponent()
{
    if (frequency->isActive())
        return frequency;
    if (logger->isActive())
        return logger;
    if (oscilloscope->isActive())
        return oscilloscope;

    throw std::exception();
}

void
Lenlab::send(const usb::pMessage &cmd)
{
    handler->send(cmd);
}

void
Lenlab::on_reply(const usb::pMessage &reply)
{
    qDebug("on_reply");
    Command cmd = reply->getCommand();

    if (cmd == init) {
        ignore = false;
    }

    if (ignore) return;

    if (cmd == startLogger)
        logger->receive(reply);
}

void
Lenlab::on_error()
{
    ignore = true;
}

void
Lenlab::on_ready()
{
    qDebug("on_ready");

    usb::pMessage cmd(new usb::Message());
    cmd->setCommand(init);
    send(cmd);

    frequency->ready();
    logger->ready();
    oscilloscope->ready();
    signal->ready();
}

} // namespace model
