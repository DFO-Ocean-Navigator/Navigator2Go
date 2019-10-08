#ifndef TSERVERMANAGER_H
#define TSERVERMANAGER_H

#include <QObject>

namespace Test {

/***********************************************************************************/
class TServerManager : public QObject {
  Q_OBJECT

private slots:

  void check_QObjectIsParent();

  void check_noCopyNoMove();
};

} // namespace Test

#endif // TSERVERMANAGER_H
