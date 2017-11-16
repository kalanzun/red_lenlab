#ifndef MINMAXVECTOR_H
#define MINMAXVECTOR_H

#include <QVector>

namespace model {

class MinMaxVector : public QVector<double>
{
public:
    void append(double value);

    void clear();

    double getMin();
    double getMax();

private:
    typedef QVector<double> super;

    double min = 0;
    double max = 0;
};

} // namespace model

#endif // MINMAXVECTOR_H
