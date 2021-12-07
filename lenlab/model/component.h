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

    virtual QVector< QString > channel_names() = 0;

signals:

};

} // namespace model

#endif // MODEL_COMPONENT_H
