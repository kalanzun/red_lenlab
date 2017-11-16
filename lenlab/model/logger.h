#ifndef LOGGER_H
#define LOGGER_H

#include "component.h"
#include <QObject>
#include <QVector>

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

    virtual void receive(const usb::pMessage &reply);

signals:
    void replot();

public slots:

private:
    typedef Component super;

    uint32_t interval = 1000;
    std::array<QVector<double>, 5> data;
};

} // namespace model

#endif // LOGGER_H
