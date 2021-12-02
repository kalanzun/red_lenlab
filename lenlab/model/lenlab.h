#ifndef MODEL_LENLAB_H
#define MODEL_LENLAB_H

#include <QObject>

#include "protocol/board.h"
#include "logger.h"

namespace model {

class Lenlab : public QObject
{
    Q_OBJECT

    protocol::Board *board;

    Logger *logger;

public:
    explicit Lenlab(QObject *parent = nullptr);

    void lookForDevice();

signals:

};

} // namespace model

#endif // MODEL_LENLAB_H
