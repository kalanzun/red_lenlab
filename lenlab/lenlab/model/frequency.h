#ifndef FREQUENCY_H
#define FREQUENCY_H

#include "component.h"
#include <QObject>

namespace model {

/**
 * @brief Lenlab Frequency Analysis component.
 */
class Frequency : public Component
{
    Q_OBJECT
public:
    explicit Frequency(QObject *parent = nullptr);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

signals:

public slots:

private:
    typedef Component super;
};

} // namespace model

#endif // FREQUENCY_H
