#include "spectrumanalyzer.h"

namespace model {

SpectrumAnalyzer::SpectrumAnalyzer(protocol::Board* board)
    : Component{board}
    , board{board}
{

}

QVector< QString > SpectrumAnalyzer::channel_names()
{
    QVector< QString > labels;

    labels.append(QString("Amplitude"));
    labels.append(QString("Phase"));

    return labels;
}

} // namespace model
