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
#include <QString>
#include <QDebug>
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

private:
    protocol::pManager manager;

    //int m_short_timeout = 10;
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
    manager = protocol::pManager::create();

    QSignalSpy spy(manager.data(), &protocol::Manager::ready);
    QVERIFY(spy.isValid());

    QVERIFY(spy.wait(m_long_timeout));
    QCOMPARE(spy.count(), 1);
}

void ProtocolTest::cleanupTestCase()
{
    manager.clear();
}

QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
