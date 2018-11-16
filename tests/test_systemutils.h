#ifndef TEST_SYSTEMUTILS_H
#define TEST_SYSTEMUTILS_H

#include <QObject>

namespace Test {

class Test_SystemUtils : public QObject {
	Q_OBJECT

private slots:

	void check_IsProcessRunning();
};

} // namespace Test

#endif // TEST_SYSTEMUTILS_H
