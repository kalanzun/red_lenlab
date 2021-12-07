#ifndef MODEL_SPECTRUMANALYZER_H
#define MODEL_SPECTRUMANALYZER_H

#include "component.h"

namespace model {

class SpectrumAnalyzer : public Component
{
    Q_OBJECT

    static const QStringList names;

public:
    explicit SpectrumAnalyzer(protocol::Board* board);

signals:

};

} // namespace model

#endif // MODEL_SPECTRUMANALYZER_H
