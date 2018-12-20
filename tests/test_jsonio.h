#ifndef TEST_JSONIO_H
#define TEST_JSONIO_H

#include <QObject>

namespace Test {

/***********************************************************************************/
class JSONIO : public QObject {
  Q_OBJECT

private slots:
  ///
  void loadjsonfile_returns_empty_qjsondocument_when_path_is_invalid();
  ///
  void loadjsonfile_returns_parsed_qjsondocument_when_path_is_valid();
};

} // namespace Test

#endif // TEST_JSONIO_H
