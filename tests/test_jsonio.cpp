#include "test_jsonio.h"

#include "../app/jsonio.cpp"

#include <QtTest/QtTest>

namespace Test {

/***********************************************************************************/
void JSONIO::loadjsonfile_returns_empty_qjsondocument_when_path_is_invalid() {
  const auto &path{QStringLiteral("/random/path")};
  const auto &doc{IO::LoadJSONFile(path, false)};

  QCOMPARE(true, doc.isEmpty());
}

/***********************************************************************************/
void JSONIO::loadjsonfile_returns_parsed_qjsondocument_when_path_is_valid() {
  const auto &path{QStringLiteral(":fixtures/sample_json.txt")};
  const auto &doc{IO::LoadJSONFile(path, false)};

  QCOMPARE(false, doc.isEmpty());
}

} // namespace Test
