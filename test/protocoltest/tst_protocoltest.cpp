#include "usb/bus.h"
#include "lenlab_version.h"
#include <QString>
#include <QDebug>
#include <QtTest>

// add necessary includes here

class ProtocolTest : public QObject
{
    Q_OBJECT

public:
    ProtocolTest();
    ~ProtocolTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_getName();
    void test_getVersion();
    void test_setSignalSine();
    void test_startOscilloscope();
    void test_startOscilloscopeTrigger();

private:
    usb::Bus bus;
    QSharedPointer<usb::Device> device;

    int m_short_timeout = 10;
    int m_long_timeout = 100;
};

ProtocolTest::ProtocolTest() : bus()
{

}

ProtocolTest::~ProtocolTest()
{

}

void ProtocolTest::initTestCase()
{
    device = bus.query(LENLAB_VID, LENLAB_PID);
    QVERIFY(device.data()); // device found?
}

void ProtocolTest::cleanupTestCase()
{
    device.clear();
}

void ProtocolTest::test_getName()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pMessage)));
    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(getName);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));
    QString reply_name(reply->getString());
    auto name = QString("Lenlab red Firmware Version %1.%2.").arg(MAJOR).arg(MINOR);
    QVERIFY(reply_name.startsWith(name));

    QVERIFY(spy.wait(m_short_timeout) == 0);
}

void ProtocolTest::test_getVersion()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pMessage)));
    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(getVersion);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));
    uint32_t *array = reply->getIntArray(3);
    QCOMPARE(array[0], MAJOR);
    QCOMPARE(array[1], MINOR);
    //QCOMPARE(array[2], REVISION);

    QVERIFY(spy.wait(m_short_timeout) == 0);
}

void ProtocolTest::test_setSignalSine()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pMessage)));
    QVERIFY(spy.isValid());
    /*
    auto cmd = usb::newCommand(setSignalSine);
    cmd->setInt(0); // multiplier
    cmd->setInt(0); // predivider
    cmd->setInt(0); // divider
    cmd->setInt(0); // amplitude
    cmd->setInt(0); // second
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));
    QCOMPARE(reply->getReply(), SignalSine);
    */
    QVERIFY(spy.wait(m_short_timeout) == 0);
}


void ProtocolTest::test_startOscilloscope()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pMessage)));
    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(startOscilloscope);
    cmd->setInt(0); // samplerate
    device->send(cmd);

    // 16 OscilloscopeData replies
    for (int i = 0; i < 16; i++) {
        QVERIFY(spy.wait(m_short_timeout));
        QCOMPARE(spy.count(), i+1);

        auto reply = qvariant_cast<usb::pMessage>(spy.at(i).at(0));
        QCOMPARE(reply->getReply(), OscilloscopeData);
        QCOMPARE(reply->getType(), ShortArray);
    }

    QVERIFY(spy.wait(m_short_timeout) == 0);
}

void ProtocolTest::test_startOscilloscopeTrigger()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pMessage)));
    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(startOscilloscopeTrigger);
    cmd->setInt(0); // samplerate
    device->send(cmd);

    QVERIFY(spy.wait(m_long_timeout));

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));
    QCOMPARE(reply->getReply(), OscilloscopeData);
    QCOMPARE(reply->getType(), ByteArray);

    // 16 OscilloscopeData replies
    for (int i = 1; i < 18; i++) {
        QVERIFY(spy.wait(m_short_timeout));

        auto reply = qvariant_cast<usb::pMessage>(spy.at(i).at(0));
        QCOMPARE(reply->getReply(), OscilloscopeData);
        QCOMPARE(reply->getType(), ByteArray);
    }

    QVERIFY(spy.wait(m_short_timeout) == 0);
}

QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
