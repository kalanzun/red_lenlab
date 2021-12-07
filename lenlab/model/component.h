#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include <QObject>

#include "protocol/board.h"

namespace model {

class Component : public QObject
{
    Q_OBJECT

protected:
    protocol::Board* board;

public:
    explicit Component(protocol::Board* board);

signals:

};

} // namespace model

#endif // MODEL_COMPONENT_H
