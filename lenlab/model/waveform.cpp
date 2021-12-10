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
    return {*x, *y};
}

PointIterator& PointIterator::operator++()
{
    ++x;
    ++y;
    return *this;
}


Channel::Channel(int channel, Waveform* waveform)
    : channel{channel}
    , waveform{waveform}
{

}

PointIterator Channel::begin()
{
    return PointIterator(std::move(waveform->x_values.constBegin()), std::move(waveform->y_values[channel].constBegin()));
}

PointIterator Channel::end()
{
    return PointIterator(std::move(waveform->x_values.constEnd()), std::move(waveform->y_values[channel].constEnd()));
}

Waveform::Waveform(QObject *parent)
    : QObject{parent}
{
    x_values.reserve(5000);

    for (int i = 0; i < channels; ++i) {
        y_values[i].reserve(5000);
    }
}

void Waveform::appendSample(struct Sample& sample)
{
    x_values.append(sample.x);

    for (int i = 0; i < channels; ++i) {
        y_values[i].append(sample.y[i]);
    }

    emit SampleAppended(sample);
}

int Waveform::getLength()
{
    return x_values.size();
}

Channel Waveform::getChannel(int channel)
{
    return Channel(channel, this);
}

const char* const Waveform::delimiter = ", ";

void Waveform::csv(std::ostream& out)
{
    // TODO QVector iterator
    for (int i = 0; i < x_values.size(); ++i) {
        out << x_values[i];
        for (int c = 0; c < channels; ++c) {
            out << delimiter << y_values[c][i];
        }
        out << "\n";
    }
}

} // namespace model
