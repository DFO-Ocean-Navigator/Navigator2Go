#include "updaterunnable.h"

#include <QProcess>

/***********************************************************************************/
UpdateRunnable::UpdateRunnable(const QString& ONInstallDir) : QRunnable{}, m_ONInstallDir{ONInstallDir} {

}

/***********************************************************************************/
void UpdateRunnable::run() {
	QProcess p;
	p.setProgram("/bin/sh");
	p.setWorkingDirectory(m_ONInstallDir);
}
