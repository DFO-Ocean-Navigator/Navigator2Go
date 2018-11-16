#ifndef TEST_IOUTILS_H
#define TEST_IOUTILS_H

#include "../app/ioutils.h"

namespace Test {

/***********************************************************************************/
class IOUtils : public QObject {
	Q_OBJECT

private slots:

	/***********************************************************************************/
	void test_constants();

};

} // namespace Test
#endif // TEST_IOUTILS_H
