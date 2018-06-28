#include <QString>
#include <QtTest>

class NetworkTest : public QObject {
	Q_OBJECT

public:
	NetworkTest();

private Q_SLOTS:
	void testCase1();
};

NetworkTest::NetworkTest() {
}

void NetworkTest::testCase1() {
	QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(NetworkTest)

#include "tst_networktest.moc"
