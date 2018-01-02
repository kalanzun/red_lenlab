# Unittesting in Qt

## Links

http://doc.qt.io/qt-5/qttest-index.html

http://doc.qt.io/qt-5/qtest-overview.html

http://doc.qt.io/qt-5/qttestlib-tutorial1-example.html

http://doc.qt.io/qt-5/qttestlib-tutorial2-example.html

http://doc.qt.io/qt-5/qttestlib-tutorial3-example.html

http://doc.qt.io/qt-5/qttestlib-tutorial4-example.html

http://doc.qt.io/qt-5/qttestlib-tutorial5-example.html

http://doc.qt.io/qt-5/qtest-overview.html

http://doc.qt.io/qtcreator/creator-autotest.html

## Project Structure

https://stackoverflow.com/questions/12154980/how-to-structure-project-while-unit-testing-qt-app-by-qtestlib

* Two separate projects sharing a configuration file (.pri)
* A single project with a project file that supports normal mode (release and debug) and test mode
* A subdirs project with a subproject for the app and a subproject for the tests

### Single Project

QtCreator Test GUI does not support this structure, the test GUI is not available.



```q
test{
    message(Configuring test build...)

    TARGET = lenlabtests

    QT += testlib

    SOURCES -= main.cpp

    HEADERS += \
        test/firmwaretest.h

    SOURCES += \
        test/firmwaretest.cpp
}
else{
    TARGET = lenlab
}
```



```c++
#ifndef FIRMWARETEST_H
#define FIRMWARETEST_H

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QSignalSpy>
#include "usb/handler.h"

class FirmwareTest : public QObject
{
    Q_OBJECT
public:
    explicit FirmwareTest(QObject *parent = nullptr);

signals:

public slots:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();

private:
    usb::Handler handler;
    QSignalSpy spy;
};

#endif // FIRMWARETEST_H
	
```



```c++
#include "firmwaretest.h"

FirmwareTest::FirmwareTest(QObject *parent) : QObject(parent), handler(), spy(&handler, SIGNAL(ready))
{

}

void FirmwareTest::initTestCase()
{
}

void FirmwareTest::cleanupTestCase()
{
}

void FirmwareTest::testCase1()
{
    QCOMPARE(spy.count(), 1);
    //QVERIFY2(true, "Failure");
}

QTEST_MAIN(FirmwareTest)

```



### Subdirs Project

QtCreator Test GUI supports this structure, the test GUI works.

* subdirs project
  * qt project directory for app
  * subdirs project test
    * unit test project for each test case

QtCreator GUI for test: Alt 8

Creating sub projects: right click on parent project and add subproject

## Further Reading

https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

