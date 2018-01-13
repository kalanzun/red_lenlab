#include "usb/handler.h"
#include "usb/message.h"
#include "lenlab_version.h"
#include <QString>
#include <QDebug>
#include <QtTest>
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
};

FirmwareTest::FirmwareTest()
{
}

void FirmwareTest::initTestCase()
{
    handler.reset(new usb::Handler());

    QSignalSpy spy(handler.data(), SIGNAL(ready()));

    QVERIFY(spy.isValid());

    QVERIFY(spy.wait(500));
}

void FirmwareTest::cleanupTestCase()
{
    handler.clear();
}

/*
void FirmwareTest::testSignalGeneratorFrequency_data()
{
    QTest::addColumn<uint32_t>("frequency");
    for (uint32_t i = 1; i < 10; i++)
        QTest::newRow(QString("%1 Hz").arg(10*i).toUtf8().constData()) << 10*i;
    for (uint32_t i = 1; i < 10; i++)
        QTest::newRow(QString("%1 Hz").arg(100*i).toUtf8().constData()) << 100*i;
    for (uint32_t i = 1; i < 10; i++)
        QTest::newRow(QString("%1 kHz").arg(1*i).toUtf8().constData()) << 1000*i;
    QTest::newRow(QString("10 kHz").toUtf8().constData()) << 10000u;
}

void FirmwareTest::testSignalGeneratorFrequency()
{
    QFETCH(uint32_t, frequency);

    auto cmd = usb::newCommand(testSignalgeneratorSineFrequency);
    *((uint16_t *) (cmd->getPayload())) = frequency;
    cmd->setPayloadLength(2);
    handler->send(cmd);
    //QFETCH(QString, data);
    //QVERIFY2(true, "Failure");

    QSignalSpy spy(handler.data(), SIGNAL(reply(pMessage)));

    QVERIFY(spy.isValid());

    QVERIFY(spy.wait(500));

    usb::pMessage reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));

    uint32_t a = *((uint32_t *) (reply->getPayload() + 0));
    uint32_t b = *((uint32_t *) (reply->getPayload() + 4));

    uint32_t sg_frequency = b * 80000000 / 16 / 500 / 2 / a;

    double relative_frequency_error = (((double) abs(sg_frequency - frequency)) / ((double) (frequency)));

    //qDebug() << a << b << result;

    QVERIFY2(a >= 4, "DAC frequency limit of 20 MHz exceeded");

    QVERIFY2(relative_frequency_error < 0.03, QString("f = %1; f_sg = %2; a = %3; b = %4;").arg(frequency).arg(sg_frequency).arg(a).arg(b).toUtf8().constData());
}
*/

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
    std::complex<double> result;
    double value;
    std::complex<double> reference[8000];

    double pi = std::acos(-1);

    for (uint32_t i = 0; i < 8000; i++) {
        double x = 2 * pi * ((double) i) / 5000.0;
        reference[i] = std::cos(x) - 1i * std::sin(x);
    }

    QSignalSpy spy(handler.data(), SIGNAL(reply(pMessage)));

    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(startOscilloscope);
    handler->send(cmd);

    do {
    QVERIFY(spy.wait(500));
    } while(spy.count() < 16);

    result = 0;

    for (uint32_t b = 0; b < 16; b+=2) {
        auto reply = qvariant_cast<usb::pMessage>(spy.at(b).at(0));

        QCOMPARE(reply->getCommand(), startOscilloscope);

        uint8_t *buffer = reply->getBody() + 6;

        for (uint32_t i = 0; i < 1000; i++) {
            result += (((double) buffer[i]) / 128.0 - 1.0) * reference[1000*b/2+i];
        }
    }

    value = 2.0 * std::abs(result) / 8000.0;

    qDebug() << value;

    QVERIFY(value > 0.8);

}

QTEST_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
