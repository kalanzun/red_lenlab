#ifndef MODEL_WAVEFORM_H
#define MODEL_WAVEFORM_H

#include <iostream>

#include <QObject>
#include <QPointF>

namespace model {

struct Point
{
    float x;
    float y;
};

struct Sample
{
    static const int channels = 4;
    float x;
    float y[channels];
};

class Waveform;

class PointIterator
{
    QVector< float >::const_iterator x;
    QVector< float >::const_iterator y;

public:
    explicit PointIterator(QVector< float >::const_iterator x, QVector< float >::const_iterator y);

    PointIterator(const PointIterator&) = delete;
    const PointIterator& operator=(const PointIterator&) = delete;

    bool operator!=(const PointIterator& other);
    struct Point operator*();
    PointIterator& operator++();
};

class Channel
{
    const int channel;
    const Waveform* waveform;

public:
    explicit Channel(int channel, const Waveform* waveform);

    Channel(const Channel&) = delete;
    const Channel& operator=(const Channel&) = delete;

    PointIterator begin() const;
    PointIterator end() const;
};

class Waveform : public QObject
{
    Q_OBJECT

public:
    static const int channels = Sample::channels;
    static const char* const delimiter;
    uint32_t interval = 0;

    QVector< float > x_values;
    QVector< float > y_values[channels];

    explicit Waveform(QObject *parent = nullptr);

    void appendSample(const struct Sample& sample);

    int getLength() const;
    Channel getChannel(int channel) const;

    void csv(std::ostream& out) const;

signals:
    void SampleAppended(const struct Sample& sample);
};

} // namespace model

#endif // MODEL_WAVEFORM_H
