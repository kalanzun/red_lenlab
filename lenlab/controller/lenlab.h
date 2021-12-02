#ifndef CONTROLLER_LENLAB_H
#define CONTROLLER_LENLAB_H

#include <QObject>

#include "manager.h"

namespace controller {

class Lenlab : public QObject
{
    Q_OBJECT

    Manager *manager;

public:
    explicit Lenlab(QObject *parent = nullptr);

    void lookForBoard();

signals:

};

} // namespace controller

#endif // CONTROLLER_LENLAB_H
