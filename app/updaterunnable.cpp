#include "updaterunnable.h"

#include <QProcess>

#ifdef QT_DEBUG
#include <QDebug>
#endif

/***********************************************************************************/
UpdateRunnable::UpdateRunnable(const QString &ONInstallDir)
    : QRunnable{}, m_ONInstallDir{ONInstallDir} {}

/***********************************************************************************/
void UpdateRunnable::run() {
  QProcess p;
  p.setProgram(QStringLiteral("/bin/sh"));
  p.setWorkingDirectory(m_ONInstallDir);
  p.start(QStringLiteral("git"), {QStringLiteral("fetch")});
  p.waitForFinished();

  p.start(QStringLiteral("git"), {QStringLiteral("pull")});
  p.waitForFinished();

  if (const QString output{p.readAllStandardOutput()};
      output.contains(QStringLiteral("Already up-to-date"))) {
#ifdef QT_DEBUG
    qDebug() << output;
    qDebug() << "Nothing to update.";
#endif
    return;
  }

  p.setWorkingDirectory(m_ONInstallDir +
                        QStringLiteral("/oceannavigator/frontend/"));
  p.start(QStringLiteral("npm"), {QStringLiteral("install")});
  p.waitForFinished();
#ifdef QT_DEBUG
  qDebug() << p.readAllStandardOutput();
#endif

  p.start(QStringLiteral("npm"),
          {QStringLiteral("run"), QStringLiteral("build")});
  p.waitForFinished(3000000);
#ifdef QT_DEBUG
  qDebug() << p.readAllStandardOutput();
#endif

  emit finished();
}
