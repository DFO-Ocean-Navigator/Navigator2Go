#ifndef TNETCDFIMPORTDESC_H
#define TNETCDFIMPORTDESC_H

#include <QObject>

namespace Test {

class TNetCDFImportDesc : public QObject {
  Q_OBJECT

private slots:
  void check_noexceptCtor();

  void check_const();

  void check_types();
};

} // namespace Test

#endif // TNETCDFIMPORTDESC_H
