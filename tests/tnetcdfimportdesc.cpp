#include "tnetcdfimportdesc.h"

#include "../app/netcdfimportdesc.h"

#include <QtTest/QtTest>

namespace Test {

/***********************************************************************************/
void TNetCDFImportDesc::check_noexceptCtor() {

	QCOMPARE( noexcept(::NetCDFImportDesc()), true);
}

/***********************************************************************************/
void TNetCDFImportDesc::check_const() {

	QCOMPARE( ::std::is_const_v<decltype(::NetCDFImportDesc::File)>, true);
	QCOMPARE( ::std::is_const_v<decltype(::NetCDFImportDesc::DatasetToAppendTo)>, true);
}

/***********************************************************************************/
void TNetCDFImportDesc::check_types() {

	QCOMPARE( (::std::is_same_v<
								::std::remove_const_t<decltype(::NetCDFImportDesc::File)>,
								QString>),
			  true);

	QCOMPARE( (::std::is_same_v<
								::std::remove_const_t<decltype(::NetCDFImportDesc::DatasetToAppendTo)>,
								QString>),
			  true);
}

} // namespace Test
