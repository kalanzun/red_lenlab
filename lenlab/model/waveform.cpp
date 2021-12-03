#include "waveform.h"

namespace model {

ChannelIterator::ChannelIterator(QVector< float >::const_iterator x, QVector< float >::const_iterator y)
    : x{std::move(x)}
    , y{std::move(y)}
{

}

bool ChannelIterator::operator!=(const ChannelIterator& other)
{
    return x != other.x;
}

struct Point ChannelIterator::operator*()
{
    return {*x, *y};
}

ChannelIterator& ChannelIterator::operator++()
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

ChannelIterator Channel::begin()
{
    return ChannelIterator(std::move(waveform->x_values.constBegin()), std::move(waveform->y_values[channel].constBegin()));
}

ChannelIterator Channel::end()
{
    return ChannelIterator(std::move(waveform->x_values.constEnd()), std::move(waveform->y_values[channel].constEnd()));
}

Waveform::Waveform(QObject *parent)
    : QObject{parent}
{

}

void Waveform::append(float x, float* y, int length)
{
    x_values.append(x);

    for (int i = 0; i < channels && i < length; ++i) {
        y_values[i].append(y[i]);
    }
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
