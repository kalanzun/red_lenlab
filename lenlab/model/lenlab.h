#ifndef LENLAB_H
#define LENLAB_H

#include "frequency.h"
#include "logger.h"
#include "oscilloscope.h"
#include "signal.h"
#include <QObject>

namespace model {

/**
 * @brief Root object of the Lenlab data model.
 */

class Lenlab : public QObject
{
    Q_OBJECT
public:
    explicit Lenlab(QObject *parent = nullptr);

    bool isActive();
    Component *getActiveComponent();

    void send();

    Frequency *frequency;
    Logger *logger;
    Oscilloscope *oscilloscope;
    Signal *signal;

signals:
    void command();

public slots:
    void reply();

private:
};

} // namespace model

#endif // LENLAB_H
