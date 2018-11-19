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
#include <QString>
#include <QDebug>
#include <QtTest>

class USBTest : public QObject
{
    Q_OBJECT

public:
    USBTest();
    ~USBTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_getName();
    void test_getVersion();
    void test_setSignalSine();
    void test_startOscilloscope();
    void test_startOscilloscopeTrigger();

private:
    usb::Bus bus;
    usb::pDevice device;

    int m_short_timeout = 10;
    //int m_long_timeout = 800;
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
    QVERIFY(device.data());
}

void USBTest::cleanupTestCase()
{
    device.clear();
}

void USBTest::test_getName()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = usb::pPacket::create();
    cmd->getByteBuffer()[0] = getName;
    cmd->setByteLength(4);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pPacket>(spy.at(0).at(0));
    QString reply_name(reinterpret_cast<const char *>(reply->getByteBuffer() + 2));
    qDebug() << reply_name;

    auto name = QString("Lenlab red Firmware Version %1.%2.").arg(MAJOR).arg(MINOR);
    QVERIFY(reply_name.startsWith(name));

    QVERIFY(spy.wait(m_short_timeout) == 0);
}

void USBTest::test_getVersion()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());

    auto cmd = usb::pPacket::create();
    cmd->getByteBuffer()[0] = getVersion;
    cmd->setByteLength(4);
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pPacket>(spy.at(0).at(0));
    uint32_t *array = reply->getBuffer() + 1;
    QCOMPARE(array[0], MAJOR);
    QCOMPARE(array[1], MINOR);
    //QCOMPARE(array[2], REVISION);

    QVERIFY(spy.wait(m_short_timeout) == 0);
}

void USBTest::test_setSignalSine()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());
    /*
    auto cmd = usb::newCommand(setSignalSine);
    cmd->setInt(0); // multiplier
    cmd->setInt(0); // predivider
    cmd->setInt(0); // divider
    cmd->setInt(0); // amplitude
    cmd->setInt(0); // second
    device->send(cmd);

    QVERIFY(spy.wait(m_short_timeout));
    QCOMPARE(spy.count(), 1);

    auto reply = qvariant_cast<usb::pPacket>(spy.at(0).at(0));
    QCOMPARE(reply->getReply(), SignalSine);
    */
    QVERIFY(spy.wait(m_short_timeout) == 0);
}


void USBTest::test_startOscilloscope()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());
    /*
    auto cmd = usb::newCommand(startOscilloscope);
    cmd->getIntBody()[0] = 0; // samplerate
    cmd->setByteLength(4 + 4);
    device->send(cmd);

    // 16 OscilloscopeData replies
    for (int i = 0; i < 16; i++) {
        QVERIFY(spy.wait(m_short_timeout));
        QCOMPARE(spy.count(), i+1);

        auto reply = qvariant_cast<usb::pPacket>(spy.at(i).at(0));
        //QCOMPARE(reply->getReply(), OscilloscopeData);
        //QCOMPARE(reply->getType(), ShortArray);
    }

    QVERIFY(spy.wait(m_short_timeout) == 0);
    */
}

void USBTest::test_startOscilloscopeTrigger()
{
    QSignalSpy spy(device.data(), SIGNAL(reply(pPacket)));
    QVERIFY(spy.isValid());
    /*
    auto cmd = usb::newCommand(startOscilloscopeTrigger);
    cmd->getIntBody()[0] = 0; // samplerate
    cmd->setByteLength(4 + 4);
    device->send(cmd);

    QVERIFY(spy.wait(m_long_timeout));

    auto reply = qvariant_cast<usb::pPacket>(spy.at(0).at(0));
    //QCOMPARE(reply->getReply(), OscilloscopeData);
    //QCOMPARE(reply->getType(), ByteArray);

    // 16 OscilloscopeData replies
    for (int i = 1; i < 18; i++) {
        QVERIFY(spy.wait(m_short_timeout));

        auto reply = qvariant_cast<usb::pPacket>(spy.at(i).at(0));
        //QCOMPARE(reply->getReply(), OscilloscopeData);
        //QCOMPARE(reply->getType(), ByteArray);
    }

    QVERIFY(spy.wait(m_short_timeout) == 0);
    */
}

QTEST_MAIN(USBTest)

#include "tst_usbtest.moc"
