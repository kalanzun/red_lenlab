#ifndef MODEL_FREQUENCYSWEEP_H
#define MODEL_FREQUENCYSWEEP_H

#include <QObject>

#include "protocol/board.h"

namespace model {

class FrequencySweep : public QObject
{
    Q_OBJECT

    protocol::Board* board;

public:
    explicit FrequencySweep(protocol::Board* board);

signals:

};

} // namespace model

#endif // MODEL_FREQUENCYSWEEP_H
