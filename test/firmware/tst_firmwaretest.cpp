#include "usb/handler.h"
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
    void testCase1_data();
    void testCase1();

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

void FirmwareTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void FirmwareTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
