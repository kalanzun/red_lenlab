#ifndef LOGGER_H
#define LOGGER_H

#include "component.h"
#include <QObject>

namespace model {

/**
 * @brief Lenlab Logger component.
 */

class Logger : public Component
{
    Q_OBJECT
public:
    explicit Logger(Lenlab *parent);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    uint32_t getInterval() const;
    void setInterval(uint32_t interval);

    virtual void start();

signals:

public slots:

private:
    typedef Component super;

    uint32_t interval = 1000;
};

} // namespace model

#endif // LOGGER_H
