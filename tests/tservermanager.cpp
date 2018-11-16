#include "tservermanager.h"

#include "../app/servermanager.h"
#include "helper_functions.h"

#include <QtTest/QtTest>

#include <type_traits>

namespace Test {

/***********************************************************************************/
void TServerManager::check_QObjectIsParent() {

	QCOMPARE( (::std::is_base_of_v<QObject, ::ServerManager>), true);
}

/***********************************************************************************/
void TServerManager::check_noCopyNoMove() {

	QCOMPARE( is_no_copy_no_move<::ServerManager>(), true);

}

} // namespace Test
