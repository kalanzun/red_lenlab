#include "protocol/manager.h"
#include "protocol/message.h"
#include "protocol/board.h"
#include "lenlab_protocol.h"
#include <QString>
#include <QDebug>
#include <QPointer>
#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class MeasurementTest : public QObject
{
    Q_OBJECT

public:
    MeasurementTest();
    ~MeasurementTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_startOscilloscopeLinearTestData();
    void test_startTriggerLinearTestData();

private:
    protocol::Manager manager;
    QPointer<protocol::Board> board;

    int m_short_timeout = 100;
    int m_long_timeout = 800;
};

MeasurementTest::MeasurementTest()
{

}

MeasurementTest::~MeasurementTest()
{

}

void MeasurementTest::initTestCase()
{
    QSignalSpy spy(&manager, &protocol::Manager::ready);
    QVERIFY(spy.isValid());

    manager.start();

    QVERIFY(spy.wait(m_long_timeout));

    board = qvariant_cast<QPointer<protocol::Board>>(spy.at(0).at(0));
}

void MeasurementTest::cleanupTestCase()
{

}

void MeasurementTest::test_startOscilloscopeLinearTestData()
{
    uint16_t *short_buffer;

    auto transaction = board->startOscilloscopeLinearTestData();
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(transaction->replies.count(), 20);

    unsigned int j = 0;
    for (auto reply: transaction->replies) {
        QVERIFY(reply->getReply() == OscilloscopeData);
        QVERIFY(reply->getType() == ShortArray);
        QVERIFY(reply->getUInt16BufferLength() == 510);
        short_buffer = reply->getUInt16Buffer();
        // 6 shorts preamble, 504 shorts data
        for (unsigned int i = 6; i < reply->getUInt16BufferLength(); i++) {
            if (!(short_buffer[i] == (i - 6) + (j * 504))) {
                qDebug() << i << j << short_buffer[i];
                QVERIFY(false);
            }
        }
        j = (j + 1) % 10;
    }
}

void MeasurementTest::test_startTriggerLinearTestData()
{
    int8_t *byte_buffer;

    auto transaction = board->startTriggerLinearTestData();
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(transaction->replies.count(), 18);

    for (auto reply: transaction->replies) {
        QVERIFY(reply->getReply() == OscilloscopeData);
        QVERIFY(reply->getType() == ByteArray);
        QVERIFY(reply->getInt8BufferLength() == 1020);
        byte_buffer = reply->getInt8Buffer();
        // 14 byte preamble, 1006 byte data
        for (unsigned int i = 14; i < reply->getInt8BufferLength(); i++) {
            if (!(byte_buffer[i] == 4 || byte_buffer[i] == -4)) {
                qDebug() << i << byte_buffer[i];
                QVERIFY(false);
            }
        }
    }
}

QTEST_MAIN(MeasurementTest)

#include "tst_measurementtest.moc"
