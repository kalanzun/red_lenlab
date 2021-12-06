#ifndef MODEL_LENLAB_H
#define MODEL_LENLAB_H

#include <QObject>

#include "protocol/board.h"
#include "frequencysweep.h"
#include "logger.h"
#include "oscilloscope.h"

namespace model {

class Lenlab : public QObject
{
    Q_OBJECT

    protocol::Board* board;

public:
    Logger* logger;
    Oscilloscope* oscilloscope;
    FrequencySweep* frequency;

    explicit Lenlab(QObject *parent = nullptr);

    void lookForDevice();

signals:

};

} // namespace model

#endif // MODEL_LENLAB_H
