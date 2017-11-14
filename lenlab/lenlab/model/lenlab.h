#ifndef LENLAB_H
#define LENLAB_H

#include "frequency.h"
#include "logger.h"
#include "oscilloscope.h"
#include "signal.h"
#include <QObject>

namespace model {

class Lenlab : public QObject
{
    Q_OBJECT
public:
    explicit Lenlab(QObject *parent = nullptr);

    bool isActive();
    Component *getActiveComponent();

    Frequency *frequency;
    Logger *logger;
    Oscilloscope *oscilloscope;
    Signal *signal;

signals:

public slots:

private:
};

} // namespace model

#endif // LENLAB_H
