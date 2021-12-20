#ifndef MODEL_SPECTRUMANALYZER_H
#define MODEL_SPECTRUMANALYZER_H

#include "component.h"

namespace model {

class SpectrumAnalyzer : public Component
{
    Q_OBJECT

public:
    explicit SpectrumAnalyzer(protocol::Board* board);
};

} // namespace model

#endif // MODEL_SPECTRUMANALYZER_H
