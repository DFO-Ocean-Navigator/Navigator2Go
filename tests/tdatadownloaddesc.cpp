#include "tdatadownloaddesc.h"

#include "helper_functions.h"

#include "../app/datadownloaddesc.h"
#include "../app/datadownloaddesc.cpp"

#include <QtTest/QtTest>

#include <type_traits>

/***********************************************************************************/
namespace Test {

/***********************************************************************************/
void TDataDownloadDesc::check_noexcept() {
	::DataDownloadDesc desc{};

	QCOMPARE( noexcept(desc.operator==(desc)), true);
	QCOMPARE( noexcept(qHash(desc)), true);
}

/***********************************************************************************/
void TDataDownloadDesc::check_returnType_GetAPIQuery() {
	::DataDownloadDesc desc{};

	QCOMPARE( (std::is_same_v<decltype(desc.GetAPIQuery("")), QString>), true);

}

/***********************************************************************************/
void TDataDownloadDesc::check_GetAPIQuery() {
	::DataDownloadDesc desc{};

	{
		const auto& expectedResult{ QStringLiteral("http://www.navigator.oceansdata.ca/subset/?&variables&quantum&dataset_name&time=,&should_zip=0") };
		const auto& result{ desc.GetAPIQuery(QStringLiteral("http://www.navigator.oceansdata.ca")) };
		QCOMPARE(result, expectedResult);
	}

	{
		desc.ID = QStringLiteral("giops_day");
		desc.Quantum = QStringLiteral("day");
		desc.SelectedVariables = QStringList() << QStringLiteral("votemper") << QStringLiteral("vosaline");
		desc.StartDate = QDate(2018, 1, 1);
		desc.EndDate = QDate(2018, 2, 1);

		const auto& expectedResult{ QStringLiteral("http://www.navigator.oceansdata.ca/subset/?&variables=votemper,vosaline&quantum=day&dataset_name=giops_day&time=2018-01-01,2018-02-01&should_zip=0") };
		const auto& result{ desc.GetAPIQuery(QStringLiteral("http://www.navigator.oceansdata.ca")) };

		QCOMPARE(result, expectedResult);
	}
}

} // namespace Test
