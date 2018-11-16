#include "tdatadownloaddesc.h"

#include "helper_functions.h"

#include "../app/datadownloaddesc.h"

#include <QtTest/QtTest>

#include <type_traits>

/***********************************************************************************/
namespace Test {

/***********************************************************************************/
void TDataDownloadDesc::check_noexcept() {
	::DataDownloadDesc desc;

	QCOMPARE( noexcept(desc.operator==(desc)), true);
	QCOMPARE( noexcept(qHash(desc)), true);
}

/***********************************************************************************/
void TDataDownloadDesc::check_returnType_GetAPIQuery() {
	::DataDownloadDesc desc;

	QCOMPARE( (std::is_same_v<decltype(desc.GetAPIQuery("")), QString>), true);

}

} // namespace Test
