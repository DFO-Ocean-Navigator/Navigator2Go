#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QProcess>

class ServerManager : public QObject {
	Q_OBJECT
public:
	ServerManager(QObject* parent = nullptr);
	~ServerManager();

	Q_DISABLE_COPY(ServerManager)

	void refreshServers();

	auto isWebUIRunning() const noexcept { return m_isGunicornRunning; }
	auto isTHREDDSRunning() const noexcept { return m_isApacheRunning; }

private:
	void startServers();
	void startWebServer();
	void startTHREDDS();

	void stopServers();
	void stopWebServer();
	void stopTHREDDS();

	QProcess m_apacheProcess{this}, m_gunicornProcess{this};
	bool m_isGunicornRunning{false};///< gUnicorn server
	bool m_isApacheRunning{false};	///< Apache tomcat server
	qint64 m_gunicornPID{0};		///< PID of gUnicorn process
	qint64 m_apachePID{0};			///< PID of apache tomcat process
};

#endif // SERVERMANAGER_H
