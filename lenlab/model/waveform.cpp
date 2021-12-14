#include "waveform.h"

namespace model {

PointIterator::PointIterator(QVector< float >::const_iterator x, QVector< float >::const_iterator y)
    : x{std::move(x)}
    , y{std::move(y)}
{

}

bool PointIterator::operator!=(const PointIterator& other)
{
    return x != other.x;
}

struct Point PointIterator::operator*()
{
    return Point{*x, *y};
}

PointIterator& PointIterator::operator++()
{
    ++x;
    ++y;
    return *this;
}


Channel::Channel(int channel, const Waveform* waveform)
    : channel{channel}
    , waveform{waveform}
{

}

PointIterator Channel::begin() const
{
    return PointIterator{waveform->x_values.constBegin(), waveform->y_values[channel].constBegin()};
}

PointIterator Channel::end() const
{
    return PointIterator{waveform->x_values.constEnd(), waveform->y_values[channel].constEnd()};
}

Waveform::Waveform(QObject *parent)
    : QObject{parent}
{
    x_values.reserve(5000);

    for (auto i = 0; i < channels; ++i) {
        y_values[i].reserve(5000);
    }
}

void Waveform::appendSample(const struct Sample& sample)
{
    x_values.append(sample.x);

    for (auto i = 0; i < channels; ++i) {
        y_values[i].append(sample.y[i]);
    }

    emit SampleAppended(sample);
}

int Waveform::getLength() const
{
    return x_values.size();
}

Channel Waveform::getChannel(int channel) const
{
    return Channel{channel, this};
}

const char* const Waveform::delimiter = ", ";

void Waveform::csv(std::ostream& out) const
{
    // TODO QVector iterator
    for (auto i = 0; i < x_values.size(); ++i) {
        out << x_values[i];
        for (auto c = 0; c < channels; ++c) {
            out << delimiter << y_values[c][i];
        }
        out << "\n";
    }
}

} // namespace model
