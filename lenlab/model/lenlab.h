#ifndef MODEL_LENLAB_H
#define MODEL_LENLAB_H

#include <QObject>

namespace protocol {

class Board;

} // namespace protocol

namespace model {

class Logger;
class Oscilloscope;
class SpectrumAnalyzer;

class Lenlab : public QObject
{
    Q_OBJECT

    protocol::Board* board;
    Logger* logger;
    Oscilloscope* oscilloscope;
    SpectrumAnalyzer* spectrum;

public:
    explicit Lenlab(QObject *parent = nullptr);

    void lookForDevice();

signals:

};

} // namespace model

#endif // MODEL_LENLAB_H
