#ifndef MODEL_COMPONENT_H
#define MODEL_COMPONENT_H

#include <QObject>
#include <QStringList>

namespace protocol {

class Board;

}

namespace model {

class Component : public QObject
{
    Q_OBJECT

protected:
    protocol::Board* board;

public:
    explicit Component(protocol::Board* board);

    virtual const QStringList& channel_names() = 0;

signals:

};

} // namespace model

#endif // MODEL_COMPONENT_H
