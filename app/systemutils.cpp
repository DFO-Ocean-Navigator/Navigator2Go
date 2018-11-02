#include "systemutils.h"

#include <QProcess>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

namespace System {

/***********************************************************************************/
/// Checks if a named process is running on a UNIX or Windows system
#ifdef __linux__
bool IsProcessRunning(const QString& processName) {
	static const auto& prefix{QStringLiteral("ps cax | grep ")};
	static const auto& postfix{QStringLiteral(" > /dev/null; if [ $? -eq 0 ]; then echo \"true\"; else echo \"false\"; fi")};

	QProcess process;
	process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());

	const auto args{ QStringList() << QStringLiteral("-c") << prefix + processName + postfix };
	process.start(QStringLiteral("/bin/sh"), args);
	process.waitForFinished();

	// Capture output from bash script
	const QString& output{ process.readAll() };
	if (output.contains(QStringLiteral("true"), Qt::CaseInsensitive)) {
		return true;
	}

	return false;
}

#elif _WIN32
auto IsProcessRunning(const QString& processName) {
	return false;
}
#endif

/***********************************************************************************/

void SendDesktopNotification(const QString& title, const QString& message) {

#ifdef __linux__
	// TODO: Use Navigator2Go logo instead of ubuntu one.
	const auto& command{ QStringLiteral("notify-send '%1' '%2' '-i' /usr/share/pixmaps/ubuntu-logo.png '-t' 5000").arg(title).arg(message) };
#else
#endif

#ifdef QT_DEBUG
	qDebug() << command;
#endif

	std::system(command.toStdString().c_str());
}


} // namespace System
