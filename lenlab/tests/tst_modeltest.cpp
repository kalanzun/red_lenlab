#include <QtTest>
#include <QCoreApplication>

#include "model/lenlab.h"
#include "protocol/board.h"
#include "protocol/querythread.h"
#include "usb/context.h"

class ModelTest : public QObject
{
    Q_OBJECT

public:
    ModelTest();
    ~ModelTest();

private slots:
    void test_case1();

};

ModelTest::ModelTest()
{

}

ModelTest::~ModelTest()
{

}

void ModelTest::test_case1()
{
    usb::Context context{};
    model::Lenlab lenlab{};
}

QTEST_MAIN(ModelTest)

#include "tst_modeltest.moc"
