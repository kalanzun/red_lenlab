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


Channel::Channel(const QVector<float>* x, const QVector<float>* y)
    : x{x}
    , y{y}
{

}

PointIterator Channel::begin() const
{
    return PointIterator{x->constBegin(), y->constBegin()};
}

PointIterator Channel::end() const
{
    return PointIterator{x->constEnd(), y->constEnd()};
}

Waveform::Waveform(QObject *parent)
    : QObject{parent}
{
    x.reserve(samples);

    for (auto i = 0; i < channels; ++i) {
        y[i].reserve(samples);
    }
}

void Waveform::appendSample(const struct Sample& sample)
{
    x.append(sample.x);

    for (auto i = 0; i < channels; ++i) {
        y[i].append(sample.y[i]);
    }

    emit SampleAppended(sample);
}

int Waveform::getLength() const
{
    return x.size();
}

Channel Waveform::getChannel(int channel) const
{
    return Channel{&x, &y[channel]};
}

void Waveform::setLocked()
{
    assert(!locked);
    locked = true;
    emit Locked();
}

bool Waveform::is_locked()
{
    return locked;
}

int Waveform::getInterval()
{
    return interval;
}

void Waveform::setInterval(int interval)
{
    assert(!locked);
    this->interval = interval;
}

const char* const Waveform::delimiter = ", ";

void Waveform::csv(std::ostream& out) const
{
    for (auto i = 0; i < x.size(); ++i) {
        out << x.at(i);
        for (auto c = 0; c < channels; ++c) {
            out << delimiter << y[c].at(i);
        }
        out << "\n";
    }
}

} // namespace model
