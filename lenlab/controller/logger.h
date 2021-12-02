#ifndef CONTROLLER_LOGGER_H
#define CONTROLLER_LOGGER_H

#include <QObject>

#include "manager.h"

namespace controller {

class Logger : public QObject
{
    Q_OBJECT

    Manager* manager;

public:
    explicit Logger(Manager* manager);

signals:

public slots:
    void setup(std::shared_ptr< usb::Packet > packet);
    void reply(std::shared_ptr< usb::Packet > packet);
    void teardown();

};

} // namespace controller

#endif // CONTROLLER_LOGGER_H
