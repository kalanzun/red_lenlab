#include "oscilloscope.h"

namespace model {

Oscilloscope::Oscilloscope(QObject *parent) : Component(parent)
{

}

QString
Oscilloscope::getNameNominative()
{
    return "as Oszilloskop";
}

QString
Oscilloscope::getNameAccusative()
{
    return "as Oszilloskop";
}

} // namespace model
