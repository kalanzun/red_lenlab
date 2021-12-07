#include "spectrumanalyzer.h"

#include "protocol/board.h"

namespace model {

const QStringList SpectrumAnalyzer::names = {
    "Amplitude",
    "Phase"
};

SpectrumAnalyzer::SpectrumAnalyzer(protocol::Board* board)
    : Component{board}
{

}

const QString SpectrumAnalyzer::x_label() const
{
    return QString("Frequenz [Hz]");
}

const QString SpectrumAnalyzer::y_label() const
{
    return QString("Amplitude [dB]");
}

const QString SpectrumAnalyzer::y2_label() const
{
    return QString("Phase [°]");
}

const QStringList& SpectrumAnalyzer::channel_names() const
{
    return names;
    /*
    QVector< QString > labels;

    labels.append(QString("Amplitude"));
    labels.append(QString("Phase"));

    return labels;
    */
}

} // namespace model
