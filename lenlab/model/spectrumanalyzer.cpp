#include "spectrumanalyzer.h"

namespace model {

const QStringList SpectrumAnalyzer::names = {
    "Amplitude",
    "Phase"
};

SpectrumAnalyzer::SpectrumAnalyzer(protocol::Board* board)
    : Component{board}
{

}

const QStringList& SpectrumAnalyzer::channel_names()
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
