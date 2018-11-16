#include "test_ioutils.h"

#include <QtTest/QtTest>

namespace Test {

/***********************************************************************************/
void IOUtils::test_constants() {
#ifdef __linux__
		const constexpr auto EXPECTED_TOMCAT_BIN_DIR{ "/opt/tomcat9/bin" };
		const constexpr auto EXPECTED_NAVIGATOR_FRONTEND_DIR{ "/opt/Ocean-Data-Map-Project/" };
#else
#endif

		QCOMPARE(IO::TOMCAT_BIN_DIR, EXPECTED_TOMCAT_BIN_DIR);
		QCOMPARE(IO::NAVIGATOR_FRONTEND_DIR, EXPECTED_NAVIGATOR_FRONTEND_DIR);
}


} // namespace Test
