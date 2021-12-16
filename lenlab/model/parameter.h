#ifndef MODEL_PARAMETER_H
#define MODEL_PARAMETER_H

#include <functional>

#include <QString>

namespace model {

class LabelIterator
{
    const std::function< int(int) > value;
    const QString label;

protected:
    int i;

public:
    explicit LabelIterator(const std::function< int(int) > value, const QString label, int i);

    LabelIterator(const LabelIterator&) = delete;
    const LabelIterator& operator=(const LabelIterator&) = delete;

    bool operator!=(const LabelIterator& other);
    QString operator*();
    LabelIterator& operator++();
};

class Parameter
{
    const int length;

public:
    const std::function< int(int) > value;
    const QString label;
    const int default_index;

    explicit Parameter(int length, const std::function< int(int) > value, const QString label, int default_index = 0);

    Parameter(const Parameter&) = delete;
    const Parameter& operator=(const Parameter&) = delete;

    LabelIterator begin() const;
    LabelIterator end() const;
};

} // namespace model

#endif // MODEL_PARAMETER_H
