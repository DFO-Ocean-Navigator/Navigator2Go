#include "mainwindow.h"

#include <QApplication>

/***********************************************************************************/
int main(int argc, char *argv[]) {
  const QApplication app(argc, argv);
  app.setWindowIcon(QIcon(QStringLiteral(":/icons/icons/compass.png")));
  app.setApplicationName(QStringLiteral("Navigator2Go"));
  app.setOrganizationName(QStringLiteral("Fisheries and Oceans Canada"));

  MainWindow w;
  w.show();

  return app.exec();
}
