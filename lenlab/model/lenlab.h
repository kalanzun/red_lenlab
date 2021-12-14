#ifndef MODEL_LENLAB_H
#define MODEL_LENLAB_H

#include <QObject>

namespace protocol { class Board; }

namespace model {

class Logger;
class Oscilloscope;
class SpectrumAnalyzer;

class Lenlab : public QObject
{
    Q_OBJECT

    protocol::Board* board;

public:
    Logger* logger;
    Oscilloscope* oscilloscope;
    SpectrumAnalyzer* spectrum;

    explicit Lenlab(QObject *parent = nullptr);

    void lookForDevice(bool create_virtual_device = false);

signals:

};

} // namespace model

#endif // MODEL_LENLAB_H
