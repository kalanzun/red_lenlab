#ifndef MODEL_PARAMETER_H
#define MODEL_PARAMETER_H

#include <functional>

#include <QString>

namespace model {

class LabelIterator
{
    const std::function< float(int) > value;
    const QString label;

protected:
    int i;

public:
    explicit LabelIterator(const std::function< float(int) > value, const QString label, int i);

    LabelIterator(const LabelIterator&) = delete;
    const LabelIterator& operator=(const LabelIterator&) = delete;

    bool operator!=(const LabelIterator& other);
    QString operator*();
    LabelIterator& operator++();
};

class Parameter
{
    const int length;
    const std::function< float(int) > value;
    const QString label;

public:
    explicit Parameter(int length, const std::function< float(int) > value, const QString label);

    Parameter(const Parameter&) = delete;
    const Parameter& operator=(const Parameter&) = delete;

    LabelIterator begin() const;
    LabelIterator end() const;
};

} // namespace model

#endif // MODEL_PARAMETER_H
