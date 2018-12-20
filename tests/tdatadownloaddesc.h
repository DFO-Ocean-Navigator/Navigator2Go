#ifndef TDATADOWNLOADDESC_H
#define TDATADOWNLOADDESC_H

#include <QObject>

namespace Test {

/***********************************************************************************/
class TDataDownloadDesc : public QObject {
  Q_OBJECT

private slots:
  ///
  void check_noexcept();
  ///
  void return_Type_of_GetAPIQuery_should_be_qstring();
  /// Possible flaky test, since it relies on an internet connection to Dory
  void check_GetAPIQuery();
};

} // namespace Test

#endif // TDATADOWNLOADDESC_H
