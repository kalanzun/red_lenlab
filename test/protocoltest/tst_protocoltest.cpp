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
    void test_TransactionTimeout();
    void test_TransactionLock();
    void test_startOscilloscope();
    //void test_startOscilloscopeTrigger();

private:
    protocol::Manager manager;
    QPointer<protocol::Board> board;

    int m_short_timeout = 100;
    int m_long_timeout = 800;
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
    QCOMPARE(spy.count(), 1);

    board = qvariant_cast<QPointer<protocol::Board>>(spy.at(0).at(0));
}

void ProtocolTest::cleanupTestCase()
{
    board.clear();
}

void ProtocolTest::test_TransactionTimeout()
{
    auto command = protocol::pMessage::create();
    command->setCommand(startOscilloscope);
    command->setType(IntArray);
    command->setIntBufferLength(1);
    command->getIntBuffer()[0] = 0; // samplerate

    auto transaction = board->call(command, 1);
    QSignalSpy spy(transaction.data(), &protocol::Transaction::failed);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);
}

void ProtocolTest::test_TransactionLock()
{
    auto command = protocol::pMessage::create();
    command->setCommand(startOscilloscope);
    command->setType(IntArray);
    command->setIntBufferLength(1);
    command->getIntBuffer()[0] = 0; // samplerate

    auto transaction = board->call(command, m_short_timeout);

    try {
        auto second_transaction = board->call(command, m_short_timeout);
        QVERIFY(false);
    } catch (std::exception) {
        QVERIFY(true);
    }

    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));
}

void ProtocolTest::test_startOscilloscope()
{
    auto command = protocol::pMessage::create();
    command->setCommand(startOscilloscope);
    command->setType(IntArray);
    command->setIntBufferLength(1);
    command->getIntBuffer()[0] = 0; // samplerate

    auto transaction = board->call(command, m_short_timeout);
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(m_short_timeout));
}
/*
void ProtocolTest::test_startOscilloscopeTrigger()
{
    auto command = protocol::pMessage::create();
    command->setCommand(startOscilloscopeTrigger);
    command->setType(IntArray);
    command->setIntBufferLength(1);
    command->getIntBuffer()[0] = 0; // samplerate

    auto transaction = board->call(command, 2000);
    QSignalSpy spy(transaction.data(), &protocol::Transaction::succeeded);
    QVERIFY(spy.isValid());
    QVERIFY(spy.wait(2000));
}
*/
QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
