#ifndef THREAD_H
#define THREAD_H

#include <QThread>

namespace usb {

    class Thread : public QThread
    {
        Q_OBJECT
    public:
        explicit Thread(QObject *parent = nullptr);
        virtual ~Thread();

        void run() override;

    signals:

    public slots:
    };

}

#endif // THREAD_H
