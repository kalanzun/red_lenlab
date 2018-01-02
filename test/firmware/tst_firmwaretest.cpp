#include <QString>
#include <QtTest>

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
};

FirmwareTest::FirmwareTest()
{
}

void FirmwareTest::initTestCase()
{
}

void FirmwareTest::cleanupTestCase()
{
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

QTEST_APPLESS_MAIN(FirmwareTest)

#include "tst_firmwaretest.moc"
