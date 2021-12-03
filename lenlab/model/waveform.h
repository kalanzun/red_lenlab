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

class ChannelIterator
{
    QVector< float >::const_iterator x;
    QVector< float >::const_iterator y;

public:
    explicit ChannelIterator(QVector< float >::const_iterator x, QVector< float >::const_iterator y);

    ChannelIterator(const ChannelIterator&) = delete;
    const ChannelIterator& operator=(const ChannelIterator&) = delete;

    bool operator!=(const ChannelIterator& other);
    struct Point operator*();
    ChannelIterator& operator++();
};

class Waveform;

class Channel
{
    const int channel;
    const Waveform* waveform;

public:
    explicit Channel(int channel, Waveform* waveform);

    Channel(const Channel&) = delete;
    const Channel& operator=(const Channel&) = delete;

    ChannelIterator begin();
    ChannelIterator end();

};

class Waveform : public QObject
{
    Q_OBJECT

public:
    static const int channels = 4;
    static const char* const delimiter;
    uint32_t interval = 0;

    QVector< float > x_values;
    QVector< float > y_values[channels];

    explicit Waveform(QObject *parent = nullptr);

    void append(float x, float* y, int length);

    int getLength();
    Channel getChannel(int channel);

    void csv(std::ostream& out);

signals:

};

} // namespace model

#endif // MODEL_WAVEFORM_H
