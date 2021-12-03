#ifndef MODEL_WAVEFORM_H
#define MODEL_WAVEFORM_H

#include <iostream>

#include <QObject>

namespace model {

class Waveform : public QObject
{
    Q_OBJECT

public:
    static const int channels = 4;
    static const char* const delimiter;
    uint32_t interval = 0;

    explicit Waveform(QObject *parent = nullptr);

    void append(float x, float* y, int length);
    void csv(std::ostream& out);

signals:

private:
    std::vector< float > x_values;
    std::vector< float > y_values[channels];
};

} // namespace model

#endif // MODEL_WAVEFORM_H
