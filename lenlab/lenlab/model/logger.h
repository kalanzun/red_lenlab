#ifndef LOGGER_H
#define LOGGER_H

#include "component.h"
#include <QObject>

namespace model {

class Logger : public Component
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

signals:

public slots:

private:
    typedef Component super;
};

} // namespace model

#endif // LOGGER_H
