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

    const QString x_label() const;
    const QString y_label() const;
    const QString y2_label() const;
    const QStringList& channel_names() const;

signals:

};

} // namespace model

#endif // MODEL_SPECTRUMANALYZER_H
