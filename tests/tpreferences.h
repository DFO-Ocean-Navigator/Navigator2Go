#ifndef TPREFERENCES_H
#define TPREFERENCES_H

#include <QObject>

namespace Test {
class TPreferences : public QObject {
  Q_OBJECT

private slots:

  void check_QSettingsIsParent();

  void check_noCopyNoMove();
};

} // namespace Test

#endif // TPREFERENCES_H
