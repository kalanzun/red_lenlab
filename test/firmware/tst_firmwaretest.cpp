#include "usb/handler.h"
#include "usb/message.h"
#include "model/lenlab.h"
#include "lenlab_version.h"
#include <QString>
#include <QDebug>
#include <QtTest>
#include <QTest>
#include <QCoreApplication>
#include <complex>

class FirmwareTest : public QObject
{
    Q_OBJECT

public:
    FirmwareTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testStringLength();
    void testName();
    void testSineMeasurement_data();
    void testSineMeasurement();

private:
    QSharedPointer<usb::Handler> handler;
    QSharedPointer<model::Lenlab> lenlab;
};

FirmwareTest::FirmwareTest()
{
}

void FirmwareTest::initTestCase()
{
    handler.reset(new usb::Handler());

    lenlab.reset(new model::Lenlab());
    lenlab->setHandler(handler.data());

    QSignalSpy spy(handler.data(), SIGNAL(ready()));

    QVERIFY(spy.isValid());

    QVERIFY(spy.wait(500));
}

void FirmwareTest::cleanupTestCase()
{
    handler.clear();
}

void FirmwareTest::testName()
{
    QSignalSpy spy(handler.data(), SIGNAL(reply(pMessage)));

    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(getName);
    handler->send(cmd);

    QVERIFY(spy.wait(500));

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));

    QString reply_name(reply->getString());
    auto name = QString("Lenlab red Firmware Version %1.%2.").arg(MAJOR).arg(MINOR);
    QVERIFY(reply_name.startsWith(name));
}

void FirmwareTest::testStringLength()
{
    QSignalSpy spy(handler.data(), SIGNAL(reply(pMessage)));

    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(getName);
    handler->send(cmd);

    QVERIFY(spy.wait(500));

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));

    QString reply_name(reply->getString());
    QCOMPARE(reply_name.size() + 1, (int) reply->getBodyLength());
}

void FirmwareTest::testSineMeasurement_data()
{
    QTest::addColumn<uint32_t>("data");
    for (uint32_t i = 0; i < 10; i++)
        QTest::newRow(QString("%1.").arg(i+1).toUtf8().constData()) << i;
}

void FirmwareTest::testSineMeasurement()
{
    QSignalSpy spy(lenlab->oscilloscope, SIGNAL(replot()));
    QVERIFY2(spy.isValid(), "Invalid Signal replot on lenlab->oscilloscope.");

    lenlab->oscilloscope->start();
    lenlab->oscilloscope->stop(); // single shot
    QVERIFY2(spy.wait(500), "Signal replot was not fired, the oscilloscope measurement did not complete.");

    auto waveform = lenlab->oscilloscope->getWaveform();

    QCOMPARE(waveform->getDataLength(), 7000u);
    QCOMPARE(waveform->getViewLength(), 6000u);

    std::complex<double> sum, y;
    double value, x;

    double pi = std::acos(-1);

    sum = 0;

    for (uint32_t i = 0; i < 6000; i++) {
        x = 2 * pi * ((double) i - 3000) / 3000;
        y = std::cos(x) - 1i * std::sin(x);
        sum += waveform->getValue(0, i) * y;
    }

    value = std::abs(sum) / 6000;

    QVERIFY2(value > 0.8, qPrintable(QString("Comparison to reference sine failed, value is %1").arg(value)));
}

QTEST_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
