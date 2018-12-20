#include "test_systemutils.h"

#include "../app/systemutils.cpp"

#include <QProcess>
#include <QtTest/QtTest>

#include <chrono>

namespace Test {

/***********************************************************************************/
void Test_SystemUtils::check_IsProcessRunning() {

#ifdef __linux__
  const auto &processName{QStringLiteral("Nav2GoTestProcess")};

  QProcess::startDetached(
      QStringLiteral("bash -c \" exec -a %1 ping 127.0.0.1 \" ")
          .arg(processName));

  // Pause for 1 sec to ensure ping process starts
  const std::chrono::seconds dura{1};
  std::this_thread::sleep_for(dura);

  const auto result{::System::IsProcessRunning(processName)};

  QCOMPARE(result, true);

  QProcess::startDetached(QStringLiteral("pkill -f %1").arg(processName));
#else
#error check_IsProcessRunning is not implemented in __FILE__
#endif
}

} // namespace Test
