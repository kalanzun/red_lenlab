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

#include <QtTest>

using namespace protocol;

class MeasurementTest : public QObject
{
    Q_OBJECT

    protocol::Board mBoard;

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
    QSignalSpy spy(&mBoard, &Board::ready);
    QVERIFY(spy.isValid());

    mBoard.lookForBoard();

    QVERIFY(spy.wait(m_long_timeout));
}

void MeasurementTest::cleanupTestCase()
{
}

void MeasurementTest::test_startOscilloscopeLinearTestData()
{
    uint16_t *short_buffer;

    pTask task(new Task(::startOscilloscopeLinearTestData));
    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    mBoard.startTask(task);
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(task->getSize(), 20);

    unsigned int j = 0;
    for (auto reply: task->getReplies()) {
        QVERIFY(reply->getReply() == OscilloscopeData);
        QVERIFY(reply->getType() == ShortArray);
        QVERIFY(reply->getUInt16BufferLength() == 510);
        short_buffer = reply->getUInt16Buffer();
        // 6 shorts preamble, 504 shorts data
        for (int i = 6; i < reply->getUInt16BufferLength(); ++i) {
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

    pTask task(new Task(::startTriggerLinearTestData));
    QSignalSpy spy(task.data(), &Task::succeeded);
    QVERIFY(spy.isValid());
    mBoard.startTask(task);
    QVERIFY(spy.wait(m_short_timeout));

    QCOMPARE(task->getSize(), 18);

    for (auto reply: task->getReplies()) {
        QVERIFY(reply->getReply() == OscilloscopeData);
        QVERIFY(reply->getType() == ByteArray);
        QVERIFY(reply->getInt8BufferLength() == 1020);
        byte_buffer = reply->getInt8Buffer();
        // 14 byte preamble, 1006 byte data
        for (int i = 14; i < reply->getInt8BufferLength(); ++i) {
            if (!(byte_buffer[i] == 4 || byte_buffer[i] == -4)) {
                qDebug() << i << byte_buffer[i];
                QVERIFY(false);
            }
        }
    }
}

QTEST_MAIN(MeasurementTest)

#include "tst_measurementtest.moc"
