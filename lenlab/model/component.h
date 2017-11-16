#ifndef COMPONENT_H
#define COMPONENT_H

#include <QObject>

namespace model {

/**
 * @brief Base class for Lenlab components.
 */

class Component : public QObject
{
    Q_OBJECT
public:
    explicit Component(QObject *parent = nullptr);

    bool isActive();

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    virtual void start();
    virtual void stop();

signals:

public slots:

protected:
    bool active = 0;

};

} // namespace model

#endif // COMPONENT_H
