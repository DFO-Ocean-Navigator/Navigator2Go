#include "process.h"

#include <QProcess>

namespace System {

/***********************************************************************************/
// Checks if a named process is running on a UNIX or Windows system
#ifdef __linux__
bool IsProcessRunning(const QString& processName) {
	static const QString& prefix{"ps cax | grep "};
	static const QString& postfix{" > /dev/null; if [ $? -eq 0 ]; then echo \"true\"; else echo \"false\"; fi"};

	QProcess process;
	process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());

	const auto args{ QStringList() << "-c" << prefix + processName + postfix };
	process.start("/bin/sh", args);
	process.waitForFinished();

	// Capture output from bash script
	const QString& output{ process.readAll() };
	if (output.contains("true", Qt::CaseInsensitive)) {
		return true;
	}

	return false;
}
#elif _WIN32
auto IsProcessRunning(const QString& processName) {
	return false;
}
#endif

} // namespace System
