#ifndef LOGGERSERIES_H
#define LOGGERSERIES_H

#include "series.h"

#include <QObject>
#include <QVector>

namespace model {

class Loggerseries : public Series
{
    Q_OBJECT

    typedef Series super;

    Q_PROPERTY(uint32_t interval READ interval WRITE setInterval)

    std::array< QVector<double>, 2 > data;

    uint32_t m_interval = 0;

public:
    explicit Loggerseries();

    void setInterval(uint32_t interval);
    uint32_t interval() const;

    void append(uint32_t channel, double value);

    uint32_t getLength(uint32_t channel);

    double getX(uint32_t i);
    double getY(uint32_t i, uint32_t channel);

    double getMinX();
    double getMaxX();
    double getMinY(uint32_t channel);
    double getMaxY(uint32_t channel);
};

} // namespace model

#endif // LOGGERSERIES_H
