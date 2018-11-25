/*

Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
Copyright (C) 2017 Christoph Simon and the Lenlab developer team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "protocol/manager.h"
#include "protocol/message.h"
#include "protocol/board.h"
#include "lenlab_protocol.h"
#include <QString>
#include <QDebug>
#include <QPointer>
#include <QtTest>

class ProtocolTest : public QObject
{
    Q_OBJECT

public:
    ProtocolTest();
    ~ProtocolTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_startLogger();
    void test_startOscilloscope();
    void test_startOscilloscopeTrigger();
    void test_startLoggerAgain();
    void test_startOscilloscopeAgain();
    void test_startOscilloscopeTriggerAgain();
    void test_TransactionTimeout();

private:
    protocol::Manager manager;
    QPointer<protocol::Board> board;

    int m_short_timeout = 100;
    int m_long_timeout = 800;
    int m_logger_timeout = 1100;
};

ProtocolTest::ProtocolTest()
{

}

ProtocolTest::~ProtocolTest()
{

}

void ProtocolTest::initTestCase()
{
    QSignalSpy spy(&manager, &protocol::Manager::ready);
    QVERIFY(spy.isValid());

    manager.start();

    QVERIFY(spy.wait(m_long_timeout));

    board = qvariant_cast<QPointer<protocol::Board>>(spy.at(0).at(0));
}

void ProtocolTest::cleanupTestCase()
{
    board.clear();
}

void ProtocolTest::test_startLogger()
{
    auto transaction = board->startLogger(1000);
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));

    QSignalSpy logger_spy(board, &protocol::Board::logger);
    QVERIFY(logger_spy.isValid());

    for (int i = 0; i < 2; i++) {
        qDebug() << i;
        QVERIFY(logger_spy.wait(m_logger_timeout));
    }

    auto stop_transaction = board->stopLogger();
    QSignalSpy stop_spy(stop_transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(stop_spy.isValid());
    QVERIFY(stop_spy.wait(m_short_timeout));

    QVERIFY(logger_spy.wait(m_logger_timeout) == 0); // no additional data point after stop
}

void ProtocolTest::test_startOscilloscope()
{
    auto transaction = board->startOscilloscope(0);
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(transaction->replies.count(), 20);
}

void ProtocolTest::test_startOscilloscopeTrigger()
{
    auto transaction = board->startOscilloscopeTrigger(2);
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(10000));

    QCOMPARE(transaction->replies.count(), 18);
}

void ProtocolTest::test_startLoggerAgain()
{
    test_startLogger();
}

void ProtocolTest::test_startOscilloscopeAgain()
{
    test_startOscilloscope();
}

void ProtocolTest::test_startOscilloscopeTriggerAgain()
{
    test_startOscilloscopeTrigger();
}

void ProtocolTest::test_TransactionTimeout()
{
    QVector<uint32_t> args;
    args.append(0);

    auto cmd = protocol::pMessage::create();
    cmd->setCommand(::startOscilloscope);
    cmd->setIntVector(args);

    auto transaction = protocol::pTransaction::create(cmd);
    transaction->setWatchdog(1); // very short timeout
    QSignalSpy spy(transaction.data(), &protocol::Transaction::failed);
    QVERIFY(spy.isValid());

    board->start(transaction);
    QVERIFY(spy.wait(m_short_timeout));

    QVERIFY(spy.wait(m_short_timeout) == 0); // wait for the device to clean up and unlock
}

QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
