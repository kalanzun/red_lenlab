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

    virtual const QString x_label() const = 0;
    virtual const QString y_label() const = 0;
    virtual const QString y2_label() const;
    virtual const QStringList& channel_names() const = 0;

signals:

};

} // namespace model

#endif // MODEL_COMPONENT_H
