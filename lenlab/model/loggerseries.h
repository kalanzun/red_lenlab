#ifndef LOGGERSERIES_H
#define LOGGERSERIES_H

#include "series.h"

#include <QObject>
#include <QVector>

#include <array>

namespace model {

class Loggerseries : public Series
{
    Q_OBJECT

    typedef Series super;

    Q_PROPERTY(uint32_t interval READ interval WRITE setInterval)
    uint32_t m_interval = 0;

    std::array< QVector<double>, 2 > data;

public:
    explicit Loggerseries();
    Loggerseries(Loggerseries const &) = delete;

    Loggerseries & operator=(Loggerseries const &) = delete;

    void setInterval(uint32_t interval);
    uint32_t interval() const;

    void append(std::size_t channel, double value);

    std::size_t getChannels() const;
    std::size_t getLength(std::size_t channel) const;

    double getX(std::size_t i) const;
    double getY(std::size_t i, std::size_t channel) const;

    double getMinX() const;
    double getMaxX() const;
    double getMinY(std::size_t channel) const;
    double getMaxY(std::size_t channel) const;
};

} // namespace model

#endif // LOGGERSERIES_H
