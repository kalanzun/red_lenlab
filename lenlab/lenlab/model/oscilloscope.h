#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "component.h"
#include <QObject>

namespace model {

class Oscilloscope : public Component
{
    Q_OBJECT
public:
    explicit Oscilloscope(QObject *parent = nullptr);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

signals:

public slots:

private:
    typedef Component super;
};

} // namespace model

#endif // OSCILLOSCOPE_H
