#include <QtTest>

#include "protocol/board.h"
#include "protocol/querythread.h"
#include "usb/context.h"

class ProtocolTest : public QObject
{
    Q_OBJECT

    static const int short_timeout = 10;

    usb::Context context;

public:
    ProtocolTest();
    ~ProtocolTest();

private slots:
    void test_create_virtual_device();

};

ProtocolTest::ProtocolTest()
    : context{}
{

}

ProtocolTest::~ProtocolTest()
{

}

void ProtocolTest::test_create_virtual_device()
{
    protocol::Board board{};

    QSignalSpy spy(&board, &protocol::Board::setup);
    QVERIFY(spy.isValid());

    board.lookForDevice(true);
    QVERIFY(spy.wait(short_timeout));
}

QTEST_MAIN(ProtocolTest)

#include "tst_protocoltest.moc"
