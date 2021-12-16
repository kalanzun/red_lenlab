#ifndef MODEL_LENLAB_H
#define MODEL_LENLAB_H

#include <QObject>

namespace protocol { class Board; }

namespace model {

class Logger;
class Oscilloscope;
class SignalGenerator;
class SpectrumAnalyzer;

class Lenlab : public QObject
{
    Q_OBJECT

public:
    protocol::Board* board;
    Logger* logger;
    Oscilloscope* oscilloscope;
    SignalGenerator* signal;
    SpectrumAnalyzer* spectrum;

    explicit Lenlab(QObject *parent = nullptr);

    void lookForDevice(bool create_virtual_device = false);
};

} // namespace model

#endif // MODEL_LENLAB_H
