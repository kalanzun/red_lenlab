/*
 * Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
 * Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "protocol/board.h"
#include "protocol/message.h"

#include "lenlab_protocol.h"

#include <QDebug>
#include <QString>
#include <QObject>

#include <QtTest>

using namespace protocol;

class ProtocolTest : public QObject
{
    Q_OBJECT

    Board mBoard;

    static int const m_short_timeout = 100;
    static int const m_long_timeout = 800;
    static int const m_logger_timeout = 150;
    static int const m_trigger_timeout = 400;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_startLogger();
    void test_startOscilloscope();
    void test_startTrigger();
    void test_startLogger2();
    void test_startLogger3();
    void test_startTrigger2();
    void test_startTrigger3();
    void test_startOscilloscope2();
    void test_startOscilloscope3();
    void test_startLogger4();
    void test_LoggerOverload();
    //void test_TransactionTimeout();
};

void ProtocolTest::initTestCase()
{
    QSignalSpy spy(&mBoard, &Board::ready);
    QVERIFY(spy.isValid());

    mBoard.lookForBoard();

    QVERIFY(spy.wait(m_long_timeout));
}

void ProtocolTest::cleanupTestCase()
{
}

void ProtocolTest::test_startLogger()
{
    QVector<uint32_t> args;
    args.append(100);

    pTask task(new Task(::startLogger));
    task->getCommand()->setUInt32Vector(args);

    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    mBoard.startTask(task);
    QVERIFY(spy.wait(m_short_timeout));

    QSignalSpy logger_spy(&mBoard, &Board::logger_data);
    QVERIFY(logger_spy.isValid());

    // reply queue length in the firmware is 4, cycle at least once
    for (int i = 0; i < 6; ++i) {
        //qDebug() << i;
        QVERIFY(logger_spy.wait(m_logger_timeout));
    }

    pTask stop_task(new Task(::stopLogger));
    QSignalSpy stop_spy(stop_task.data(), &Task::succeeded);
    QVERIFY(stop_spy.isValid());
    mBoard.startTask(stop_task);
    QVERIFY(stop_spy.wait(m_short_timeout));

    // no additional data point after stop
    QVERIFY(logger_spy.wait(m_logger_timeout) == 0);
}

void ProtocolTest::test_LoggerOverload()
{
    QVector<uint32_t> args;
    args.append(1);

    pTask task(new Task(::startLogger));
    task->getCommand()->setUInt32Vector(args);

    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    mBoard.startTask(task);
    QVERIFY(spy.wait(m_short_timeout));

    QSignalSpy logger_spy(&mBoard, &Board::logger_data);
    QVERIFY(logger_spy.isValid());

    // reply queue length in the firmware is 4, cycle at least once
    for (int i = 0; i < 10000; ++i) {
        //qDebug() << i;
        logger_spy.wait(2);
        QCOMPARE(logger_spy.count(), i + 1);
    }

    pTask stop_task(new Task(::stopLogger));
    QSignalSpy stop_spy(stop_task.data(), &Task::succeeded);
    QVERIFY(stop_spy.isValid());
    mBoard.startTask(stop_task);
    QVERIFY(stop_spy.wait(m_short_timeout));

    // no additional data point after stop
    QVERIFY(logger_spy.wait(2) == 0);
}

void ProtocolTest::test_startOscilloscope()
{
    QVector<uint32_t> args;
    args.append(1);

    pTask task(new Task(::startOscilloscope));
    task->getCommand()->setUInt32Vector(args);

    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    mBoard.startTask(task);
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(task->getSize(), 20);
}

void ProtocolTest::test_startTrigger()
{
    QVector<uint32_t> args;
    args.append(2);

    pTask task(new Task(::startTrigger, m_trigger_timeout));
    task->getCommand()->setUInt32Vector(args);

    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    mBoard.startTask(task);
    QVERIFY(spy.wait(m_trigger_timeout + m_short_timeout));

    QCOMPARE(task->getSize(), 18);
}

void ProtocolTest::test_startLogger2() { test_startLogger(); }
void ProtocolTest::test_startLogger3() { test_startLogger(); }
void ProtocolTest::test_startLogger4() { test_startLogger(); }

void ProtocolTest::test_startOscilloscope2() { test_startOscilloscope(); }
void ProtocolTest::test_startOscilloscope3() { test_startOscilloscope(); }

void ProtocolTest::test_startTrigger2() { test_startTrigger(); }
void ProtocolTest::test_startTrigger3() { test_startTrigger(); }

/*
void ProtocolTest::test_TransactionTimeout()
{
    QVector<uint32_t> args;
    args.append(0);

    auto cmd = pMessage::create();
    cmd->setCommand(::startOscilloscope);
    cmd->setIntVector(args);

    auto task = pTransaction::create(cmd);
    task->setWatchdog(1); // very short timeout
    QSignalSpy spy(task.data(), &Task::failed);
    QVERIFY(spy.isValid());

    mBoard->start(task);
    QVERIFY(spy.wait(m_short_timeout));

    QVERIFY(spy.wait(m_short_timeout) == 0); // wait for the device to clean up and unlock
}
*/

QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
