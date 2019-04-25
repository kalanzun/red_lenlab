#include "protocol/board.h"
#include "protocol/factory.h"
#include "protocol/message.h"

#include "lenlab_protocol.h"

#include <QDebug>
#include <QString>

#include <QtTest>

using namespace protocol;

class MeasurementTest : public QObject
{
    Q_OBJECT

    protocol::Factory mFactory;
    protocol::pBoard mBoard;

    static int const m_short_timeout = 100;
    static int const m_long_timeout = 800;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_startOscilloscopeLinearTestData();
    void test_startTriggerLinearTestData();
};

void MeasurementTest::initTestCase()
{
    QSignalSpy spy(&mFactory, &Factory::ready);
    QVERIFY(spy.isValid());

    mFactory.start();

    QVERIFY(spy.wait(m_long_timeout));

    mBoard = qvariant_cast<pBoard>(spy.at(0).at(0));
}

void MeasurementTest::cleanupTestCase()
{
    mBoard.clear();
}

void MeasurementTest::test_startOscilloscopeLinearTestData()
{
    uint16_t *short_buffer;

    pMessage cmd(new Message());
    cmd->setCommand(::startOscilloscopeLinearTestData);

    auto task = mBoard->startTask(cmd);
    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(task->getSize(), 20);

    unsigned int j = 0;
    for (auto reply: task->getReplies()) {
        QVERIFY(reply->getReply() == OscilloscopeData);
        QVERIFY(reply->getType() == ShortArray);
        QVERIFY(reply->getUInt16BufferLength() == 510);
        short_buffer = reply->getUInt16Buffer();
        // 6 shorts preamble, 504 shorts data
        for (unsigned int i = 6; i < reply->getUInt16BufferLength(); ++i) {
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

    pMessage cmd(new Message());
    cmd->setCommand(::startTriggerLinearTestData);

    auto task = mBoard->startTask(cmd);
    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(task->getSize(), 18);

    for (auto reply: task->getReplies()) {
        QVERIFY(reply->getReply() == OscilloscopeData);
        QVERIFY(reply->getType() == ByteArray);
        QVERIFY(reply->getInt8BufferLength() == 1020);
        byte_buffer = reply->getInt8Buffer();
        // 14 byte preamble, 1006 byte data
        for (unsigned int i = 14; i < reply->getInt8BufferLength(); ++i) {
            if (!(byte_buffer[i] == 4 || byte_buffer[i] == -4)) {
                qDebug() << i << byte_buffer[i];
                QVERIFY(false);
            }
        }
    }
}

QTEST_MAIN(MeasurementTest)

#include "tst_measurementtest.moc"
