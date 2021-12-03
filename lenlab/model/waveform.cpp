#include "waveform.h"

namespace model {

Waveform::Waveform(QObject *parent)
    : QObject{parent}
{

}

void Waveform::append(float x, float* y, int length)
{
    x_values.push_back(x);

    for (int i = 0; i < 4 && i < length; ++i) {
        y_values[i].push_back(y[i]);
    }
}

const char* const Waveform::delimiter = ", ";

void Waveform::csv(std::ostream& out)
{
    for (int i = 0; i < x_values.size(); ++i) {
        out << x_values[i];
        for (int c = 0; c < channels; ++c) {
            out << delimiter << y_values[c][i];
        }
        out << "\n";
    }
}

} // namespace model
