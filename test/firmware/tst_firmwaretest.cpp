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
    uint8_t multiplier, predivider, divider;
    double f;

    QTest::addColumn<uint8_t>("multiplier");
    QTest::addColumn<uint8_t>("predivider");
    QTest::addColumn<uint8_t>("divider");
    QTest::addColumn<double>("frequency");

    multiplier = 1;
    divider = 1;

    for (uint32_t i = 0; i < 10; i++) {
        predivider = 10 * (i+1);
        f = 5000 / predivider;
        QTest::newRow(qUtf8Printable(QString("%1 Hz").arg(f))) << multiplier << predivider << divider << f;
    }
}

void FirmwareTest::testSineMeasurement()
{
    QFETCH(uint8_t, multiplier);
    QFETCH(uint8_t, predivider);
    QFETCH(uint8_t, divider);
    QFETCH(double, frequency);

    QSignalSpy spy(lenlab->oscilloscope, SIGNAL(replot()));
    QVERIFY2(spy.isValid(), "Invalid Signal replot on lenlab->oscilloscope.");

    // SSI outputs one period in 2 channels * 500 samples * 16 bits per sample = 16000 bits
    // A 0.5 kHz sine needs a bit rate of 8 MHz
    // SysClk is 80 MHz, 80 / 8 = 10

    auto cmd = usb::newCommand(setSignalSine);
    cmd->setType(ByteArray);
    cmd->setBodyLength(3);
    cmd->getBody()[0] = multiplier; // multiplier
    cmd->getBody()[1] = predivider; // predivider
    cmd->getBody()[2] = divider; // divider
    handler->send(cmd);

    lenlab->oscilloscope->start();
    lenlab->oscilloscope->stop(); // single shot
    QVERIFY2(spy.wait(500), "Signal replot was not fired, the oscilloscope measurement did not complete.");

    auto waveform = lenlab->oscilloscope->getWaveform();

    QCOMPARE(waveform->getDataLength(), 7000u);
    QCOMPARE(waveform->getViewLength(), 6000u);

    // sample rate is 250 kHz, so one sample is 4 us

    double f = 0.9 * frequency;
    // for some reason, the frequency is 10% off

    std::complex<double> sum, y;
    double value, x;

    double pi = std::acos(-1);

    sum = 0;

    for (uint32_t i = 0; i < 6000; i++) {
        x = 2 * pi * f * 4e-6 * (double) i;
        y = std::cos(x) - 1i * std::sin(x);
        sum += waveform->getValue(0, i) * y;
    }

    value = std::abs(sum) / 6000;

    QVERIFY2(value > 0.7, qPrintable(QString("Comparison to reference sine failed, value is %1").arg(value)));
}

QTEST_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
