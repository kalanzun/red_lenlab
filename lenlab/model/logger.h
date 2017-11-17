#ifndef LOGGER_H
#define LOGGER_H

#include "component.h"
#include "minmaxvector.h"
#include <QObject>

namespace model {

/**
 * @brief Lenlab Logger component.
 */

class Logger : public Component
{
    Q_OBJECT
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)

public:
    explicit Logger(Lenlab *parent);

    virtual QString getNameNominative();
    virtual QString getNameAccusative();

    uint32_t getInterval() const;
    void setInterval(uint32_t interval);

    virtual void start();
    void clear();

    virtual void receive(const usb::pMessage &reply);

    void setAutoSave(bool autoSave);
    bool autoSave() const;

    void save(QString fileName);

    std::array<MinMaxVector, 5> data;


signals:
    void autoSaveChanged(bool);

public slots:

private:
    typedef Component super;

    void _save();
    virtual void timerEvent(QTimerEvent *event);

    uint32_t interval = 1000;
    QString fileName;
    bool m_autoSave = false;
    bool newData = false;
    int timer_id = 0;
};

} // namespace model

#endif // LOGGER_H
