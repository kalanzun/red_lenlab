#include "usb/handler.h"
#include "usb/message.h"
#include <QString>
#include <QtTest>
#include <QCoreApplication>

class FirmwareTest : public QObject
{
    Q_OBJECT

public:
    FirmwareTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSignalGeneratorFrequency_data();
    void testSignalGeneratorFrequency();

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

QTEST_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
