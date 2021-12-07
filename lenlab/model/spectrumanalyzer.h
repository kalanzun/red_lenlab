#ifndef MODEL_SPECTRUMANALYZER_H
#define MODEL_SPECTRUMANALYZER_H

#include <QObject>

#include "protocol/board.h"

namespace model {

class SpectrumAnalyzer : public QObject
{
    Q_OBJECT

    protocol::Board* board;

public:
    explicit SpectrumAnalyzer(protocol::Board* board);

signals:

};

} // namespace model

#endif // MODEL_SPECTRUMANALYZER_H
