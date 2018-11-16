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

private:
    usb::Bus bus;
    QSharedPointer<usb::Device> device;

    int short_wait = 10;
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

    QVERIFY(spy.wait(short_wait));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));
    QString reply_name(reply->getString());
    auto name = QString("Lenlab red Firmware Version %1.%2.").arg(MAJOR).arg(MINOR);
    QVERIFY(reply_name.startsWith(name));
}

void ProtocolTest::test_getVersion()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pMessage)));
    QVERIFY(spy.isValid());

    auto cmd = usb::newCommand(getVersion);
    device->send(cmd);

    QVERIFY(spy.wait(short_wait));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pMessage>(spy.at(0).at(0));
    uint32_t *array = reply->getIntArray(3);
    QCOMPARE(array[0], MAJOR);
    QCOMPARE(array[1], MINOR);
    //QCOMPARE(array[2], REVISION);
}

QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
