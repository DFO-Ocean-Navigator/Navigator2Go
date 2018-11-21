#ifndef TEST_IOUTILS_H
#define TEST_IOUTILS_H

#include <QObject>

namespace Test {

/***********************************************************************************/
class IOUtils : public QObject {
	Q_OBJECT

private slots:

	/***********************************************************************************/
	void check_constants();

};

} // namespace Test
#endif // TEST_IOUTILS_H
