#include "tpreferences.h"

#include "../app/preferences.h"
#include "helper_functions.h"

#include <QtTest/QtTest>

#include <type_traits>

namespace Test {

/***********************************************************************************/
void TPreferences::check_QSettingsIsParent() {

	QCOMPARE( (::std::is_base_of_v<::QSettings, ::Preferences>), true);
}

/***********************************************************************************/
void TPreferences::check_noCopyNoMove() {

	QCOMPARE( is_no_copy_no_move<::Preferences>(), true);
}

} // namespace Test

