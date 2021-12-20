#ifndef MODEL_PARAMETER_H
#define MODEL_PARAMETER_H

#include <functional>

#include <QObject>
#include <QString>

namespace model {

class LabelIterator
{
    const std::function< int(int) > value;
    const QString label;

protected:
    int index;

public:
    explicit LabelIterator(const std::function< int(int) > value, const QString label, int index);

    LabelIterator(const LabelIterator&) = delete;
    const LabelIterator& operator=(const LabelIterator&) = delete;

    bool operator!=(const LabelIterator& other);
    QString operator*();
    LabelIterator& operator++();
};

class Parameter : public QObject
{
    Q_OBJECT

    const int length;
    const std::function< int(int) > value;
    const QString label;

    int index;

public:
    explicit Parameter(int length, const std::function< int(int) > value, const QString label, int index = 0, QObject *parent = nullptr);

    int getLength() const;

    LabelIterator begin() const;
    LabelIterator end() const;

    int getIndex() const;

    int getValue() const;

public slots:
    void setIndex(int index);
};

} // namespace model

#endif // MODEL_PARAMETER_H
