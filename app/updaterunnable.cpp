#include "updaterunnable.h"

#include <QProcess>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
UpdateRunnable::UpdateRunnable(const QString& ONInstallDir) : QRunnable{}, m_ONInstallDir{ONInstallDir} {

}

/***********************************************************************************/
void UpdateRunnable::run() {
	QProcess p;
	p.setProgram("/bin/sh");
	p.setWorkingDirectory(m_ONInstallDir);
	p.start("git", {"fetch"});
	p.waitForFinished();

	p.start("git", {"pull"});
	p.waitForFinished();
#ifdef QT_DEBUG
	qDebug() << p.readAllStandardOutput();
#endif

	if (QString(p.readAllStandardOutput()).contains("Already up-to-date")) {
		//return;
	}

	p.setWorkingDirectory(m_ONInstallDir + "/oceannavigator/frontend/");
	p.start("npm", {"install"});
	p.waitForFinished();
#ifdef QT_DEBUG
	qDebug() << p.readAllStandardOutput();
#endif

	p.start("npm", {"run", "build"});
	p.waitForFinished(3000000);
#ifdef QT_DEBUG
	qDebug() << p.readAllStandardOutput();
#endif

	emit finished();
}
