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

#include "lenlab_protocol.h"
#include "lenlab_version.h"
#include "usb/bus.h"
#include "usb/device.h"
#include "usb/usbexception.h"
#include <QString>
#include <QDebug>
#include <QtTest>

using namespace usb;

class USBTest : public QObject
{
    Q_OBJECT

public:
    USBTest();
    ~USBTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    pPacket create_command(enum Command code, Type type);
    void set_single_int(const pPacket &cmd, const uint32_t &value);
    void verify_header(const pPacket &reply, Reply code, Type type, uint8_t error = 0, bool last = true);
    void test_bus_query_unsuccessfull();
    void test_device_construct_error();
    void test_device_send_error();
    void test_init();
    void test_getName();
    void test_getVersion();
    void test_startOscilloscopeLockError();
    void test_startOscilloscopeMemoryError();
    void test_startTriggerDMAQueue();

private:
    Bus bus;
    pDevice device;

    int m_short_timeout = 10;
    int m_long_timeout = 800;
};

USBTest::USBTest()
{

}

USBTest::~USBTest()
{

}

void USBTest::initTestCase()
{
    device = bus.query(LENLAB_VID, LENLAB_PID);
    QVERIFY(device);
}

void USBTest::cleanupTestCase()
{
    device.clear();
}

pPacket USBTest::create_command(enum Command code, enum Type type = noType)
{
    auto cmd = pPacket::create();
    auto buffer = cmd->getByteBuffer();
    buffer[0] = code;
    buffer[1] = type;
    buffer[2] = 0;
    buffer[3] = 255; // last packet
    cmd->setByteLength(LENLAB_PACKET_HEAD_LENGTH);
    return cmd;
}

void USBTest::set_single_int(const pPacket &cmd, const uint32_t &value)
{
    cmd->getBuffer()[LENLAB_PACKET_HEAD_LENGTH / 4] = value;
    cmd->setByteLength(LENLAB_PACKET_HEAD_LENGTH + 4);
}

void USBTest::verify_header(const pPacket &reply, enum Reply code, enum Type type, uint8_t error, bool last)
{
    QVERIFY(reply->getByteLength() >= 4);
    auto buffer = reply->getByteBuffer();
    QCOMPARE(buffer[0], uint8_t(code));
    QCOMPARE(buffer[1], uint8_t(type));
    if (error) QCOMPARE(buffer[2], error);
    if (last) QCOMPARE(buffer[3], uint8_t(255));
}

void USBTest::test_bus_query_unsuccessfull()
{
    auto dev = bus.query(0, 0);
    QVERIFY(!dev);
}

void USBTest::test_device_construct_error()
{
    try {
        auto dev = bus.query(LENLAB_VID, LENLAB_PID);
        // fails, because the device is already open from initTestCase
        QFAIL("bus.query did not throw USBException");
    } catch (const USBException &) {

    }
}

void USBTest::test_device_send_error()
{
    auto cmd = create_command(init);
    cmd->enableMockSendError();

    QSignalSpy spy(device.data(), SIGNAL(error(const QString &)));
    QVERIFY(spy.isValid());

    device->send(cmd); // it generates the error immediately

    QCOMPARE(spy.count(), 1);
}

void USBTest::test_init()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = create_command(init);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout)); // wait for reply
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<pPacket>(spy.at(0).at(0));
    verify_header(reply, Init, noType);

    QVERIFY(spy.wait(m_short_timeout) == 0); // no second packet
}

void USBTest::test_getName()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = create_command(getName);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout)); // wait for reply
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<pPacket>(spy.at(0).at(0));
    verify_header(reply, Name, String);

    QString reply_name(reinterpret_cast<const char *>(reply->getByteBuffer() + LENLAB_PACKET_HEAD_LENGTH));
    //qDebug() << reply_name;

    auto name = QString("Lenlab red Firmware Version %1.%2").arg(MAJOR).arg(MINOR);
    QVERIFY(reply_name.startsWith(name));

    QVERIFY(spy.wait(m_short_timeout) == 0); // no second packet
}

void USBTest::test_getVersion()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = create_command(getVersion);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout)); // wait for reply
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<pPacket>(spy.at(0).at(0));
    verify_header(reply, Version, IntArray);

    QCOMPARE(reply->getByteLength(), size_t(LENLAB_PACKET_HEAD_LENGTH + 2*4));

    uint32_t *array = reply->getBuffer() + LENLAB_PACKET_HEAD_LENGTH / 4;
    QCOMPARE(array[0], uint32_t(MAJOR));
    QCOMPARE(array[1], uint32_t(MINOR));

    QVERIFY(spy.wait(m_short_timeout) == 0); // no second packet
}

void USBTest::test_startOscilloscopeLockError()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = create_command(startOscilloscope, IntArray);
    set_single_int(cmd, 1); // samplerate

    auto sec = create_command(startOscilloscope, IntArray);
    set_single_int(sec, 1); // samplerate

    // send command
    device->send(cmd);
    // then immediately send next command
    device->send(sec);

    // which should fail
    QVERIFY(spy.wait(m_long_timeout));
    auto reply = qvariant_cast<pPacket>(spy.last().at(0));
    verify_header(reply, Error, noType, 1); // LOCK_ERROR

    // await data packages
    for (int i = 0; i < 20; ++i) {
        QVERIFY(spy.wait(m_long_timeout));
        auto reply = qvariant_cast<pPacket>(spy.last().at(0));
        verify_header(reply, OscilloscopeData, ShortArray, 0, false);
    }
    QCOMPARE(reply->getByteBuffer()[3], uint8_t(255)); // last packet
}

void USBTest::test_startTriggerDMAQueue()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = create_command(startTrigger, IntArray);
    set_single_int(cmd, 2); // samplerate

    auto sec = create_command(startTrigger, IntArray);
    set_single_int(sec, 2); // samplerate

    device->send(cmd);
    QVERIFY(spy.wait(m_long_timeout)); // wait for the first packet
    // then immediately send next command, which should queue, till the data transfer is through
    // this test sometimes fails because the firmware is too fast and the second command does not generate an error
    device->send(sec);

    // await data packages
    auto reply = qvariant_cast<pPacket>(spy.at(0).at(0));
    verify_header(reply, OscilloscopeData, ByteArray, 0, false);

    for (int i = 1; i < 18; ++i) {
        QVERIFY(spy.wait(m_long_timeout));
        reply = qvariant_cast<pPacket>(spy.at(i).at(0));
        verify_header(reply, OscilloscopeData, ByteArray, 0, false);
    }
    QCOMPARE(reply->getByteBuffer()[3], uint8_t(255)); // last packet

    // await data packages of second command
    for (int i = 0; i < 18; ++i) {
        QVERIFY(spy.wait(m_long_timeout));
        reply = qvariant_cast<pPacket>(spy.at(18+i).at(0));
        verify_header(reply, OscilloscopeData, ByteArray, 0, false);
    }
    QCOMPARE(reply->getByteBuffer()[3], uint8_t(255)); // last packet
}

void USBTest::test_startOscilloscopeMemoryError()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = create_command(startOscilloscope, IntArray);
    set_single_int(cmd, 1); // samplerate

    auto sec = create_command(startOscilloscope, IntArray);
    set_single_int(sec, 1); // samplerate

    device->send(cmd);
    QVERIFY(spy.wait(m_long_timeout)); // wait for the first packet
    // then immediately send next command, which should fail, because the second ring is still in transmission
    device->send(sec);

    // await data packages
    auto reply = qvariant_cast<pPacket>(spy.at(0).at(0));
    QCOMPARE(reply->getByteBuffer()[0], uint8_t(OscilloscopeData));

    for (int i = 1; i < 20; ++i) {
        QVERIFY(spy.wait(m_long_timeout));
        reply = qvariant_cast<pPacket>(spy.at(i).at(0));
        //qDebug() << i;
        verify_header(reply, OscilloscopeData, ShortArray, 0, false);
    }
    QCOMPARE(reply->getByteBuffer()[3], uint8_t(255)); // last packet

    // error message
    //QVERIFY(spy.wait(m_short_timeout)); // it did already arrive
    reply = qvariant_cast<pPacket>(spy.last().at(0));
    verify_header(reply, Error, noType, 3); // MEMORY_ERROR
}

QTEST_MAIN(USBTest)

#include "tst_usbtest.moc"
