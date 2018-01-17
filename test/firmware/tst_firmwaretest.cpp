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
    double f;

    QTest::addColumn<uint8_t>("index");

    for (uint8_t i = 0; i < 100; i++) {
        f = lenlab->signalgenerator->getFrequency(i);
        QTest::newRow(qUtf8Printable(QString("%1 Hz").arg(f))) << i;
    }
}

void FirmwareTest::testSineMeasurement()
{
    QFETCH(uint8_t, index);

    QSignalSpy reply_spy(handler.data(), SIGNAL(reply(pMessage)));
    QVERIFY2(reply_spy.isValid(), "Invalid Signal reply on handler.");

    QSignalSpy replot_spy(lenlab->oscilloscope, SIGNAL(replot()));
    QVERIFY2(replot_spy.isValid(), "Invalid Signal replot on lenlab->oscilloscope.");

    uint8_t divider;

    if (index >= 66)
        divider = 1;
    else if (index >= 33)
        divider = 2;
    else
        divider = 3;

    //lenlab->oscilloscope->setSamplerateDivider(divider);
    QTest::qWait(10);

    lenlab->signalgenerator->setFrequency(index);
    QVERIFY2(reply_spy.wait(500), "No confirmation for setSignalSine arrived.");
    usb::pMessage reply = qvariant_cast<usb::pMessage>(reply_spy.at(reply_spy.count()-1).at(0));

    QVERIFY(reply->getReply() == SignalSine);

    lenlab->oscilloscope->restart(); // single shot
    QVERIFY2(replot_spy.wait(500), "Signal replot was not fired, the oscilloscope measurement did not complete.");

    auto waveform = lenlab->oscilloscope->getWaveform();

    QCOMPARE(waveform->getLength(0), 7000u);
    QCOMPARE(waveform->getLength(1), 7000u);
    QCOMPARE(waveform->view(), 6000u);

    double f = lenlab->signalgenerator->getFrequency(index);

    std::complex<double> sum, y;
    double value, x;

    double pi = std::acos(-1);
    std::complex<double> i(0, 1);

    sum = 0;

    // sample rate is 1 MHz / (1<<divider), so one sample is 1 us * (1<<divider)

    for (uint32_t idx = 0; idx < 6000; idx++) {
        x = 2 * pi * f * 1e-6 * (1<<divider) * ((double) idx - 3000);
        y = std::sin(x) + i * std::cos(x);
        sum += waveform->getY(idx, 0) * y;
    }

    value = std::abs(sum) / 6000;

    qDebug() << value;

    QVERIFY2(value > 0.7, qPrintable(QString("Comparison to reference sine failed, value is %1").arg(value)));





}

QTEST_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
