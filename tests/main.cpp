#include <QtTest/QtTest>

#include "test_network.h"
#include "test_ioutils.h"
#include "tdatadownloaddesc.h"
#include "tpreferences.h"
#include "tnetcdfimportdesc.h"
#include "test_systemutils.h"

/***********************************************************************************/
int main(int argc, char** argv) {
	int status{ 0 };

	const auto& ASSERT_TEST = [&status, argc, argv](QObject* obj) {
		status |= QTest::qExec(obj, argc, argv);
		delete obj;
	};

	ASSERT_TEST(new Test::Network());
	ASSERT_TEST(new Test::IOUtils());
	ASSERT_TEST(new Test::TDataDownloadDesc());
	ASSERT_TEST(new Test::TPreferences());
	ASSERT_TEST(new Test::TNetCDFImportDesc());
	ASSERT_TEST(new Test::Test_SystemUtils());

	return status;
}
