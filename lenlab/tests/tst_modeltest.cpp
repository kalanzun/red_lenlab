#include <memory>

#include <QtTest>

#include "model/lenlab.h"
#include "model/logger.h"
#include "model/oscilloscope.h"
#include "model/waveform.h"
#include "protocol/board.h"
#include "usb/context.h"

namespace model {

class ModelTest : public QObject
{
    Q_OBJECT

    static const int short_timeout = 10;

    std::unique_ptr< usb::Context > context;
    Lenlab* lenlab;

public:
    ModelTest();
    ~ModelTest();

private slots:
    void initTestCase();
    void test_logger();
    void test_oscilloscope();

};

ModelTest::ModelTest()
    : context{std::make_unique< usb::Context >()}
    , lenlab{new Lenlab{this}}
{

}

ModelTest::~ModelTest()
{

}

void ModelTest::initTestCase()
{
    QSignalSpy spy(lenlab->board, &protocol::Board::setup);
    QVERIFY(spy.isValid());

    lenlab->lookForDevice(true);
    QVERIFY(spy.wait(short_timeout));
}

void ModelTest::test_logger()
{
    auto logger = lenlab->logger;

    logger->interval->setIndex(0);
    QCOMPARE(logger->interval->getValue(), 100); // ms

    QSignalSpy spy(logger->waveform, &Waveform::SampleAppended);
    QVERIFY(spy.isValid());

    logger->start();
    QVERIFY(spy.wait(100 + short_timeout)); // ms

    logger->stop();
}

void ModelTest::test_oscilloscope()
{
    auto osci = lenlab->oscilloscope;

    osci->samplerate->setIndex(0);
    QCOMPARE(osci->samplerate->getValue(), 1000); // kHz

    QSignalSpy spy(osci, &Component::WaveformCreated);
    QVERIFY(spy.isValid());

    osci->start();
    QVERIFY(spy.wait(100 + short_timeout)); // ms
}

} // namespace model

QTEST_MAIN(model::ModelTest)

#include "tst_modeltest.moc"
