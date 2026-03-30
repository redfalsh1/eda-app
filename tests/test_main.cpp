#include <QTest>
#include <QDebug>

class TestBasic : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "Test initialization";
    }
    
    void testBasic() {
        QVERIFY2(true, "Basic test passed");
    }
    
    void testVersion() {
        QString version = EDA_VERSION;
        QVERIFY2(!version.isEmpty(), "Version should not be empty");
        qDebug() << "Version:" << version;
    }
    
    void cleanupTestCase() {
        qDebug() << "Test cleanup";
    }
};

QTEST_APPLESS_MAIN(TestBasic)
#include "test_main.moc"
