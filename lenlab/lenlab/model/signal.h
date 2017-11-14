#ifndef SIGNAL_H
#define SIGNAL_H

#include "component.h"
#include <QObject>

namespace model {

class Signal : public Component
{
    Q_OBJECT
public:
    explicit Signal(QObject *parent = nullptr);

signals:

public slots:

private:
    typedef Component super;
};

} // namespace model

#endif // SIGNAL_H
