#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "nodiscard.h"

#include <QProcess>

/***********************************************************************************/
/// Manages the Apache and gUnicorn servers for the Ocean Navigator
class ServerManager : public QObject {
  Q_OBJECT

public:
  explicit ServerManager(QObject *parent = nullptr);
  ServerManager(const bool autoStartServers = true, QObject *parent = nullptr);

  ~ServerManager() override;

  Q_DISABLE_COPY(ServerManager)

  /// Restarts the Apache and gUnicorn servers after 5 seconds.
  void refreshServers();

  NODISCARD auto isWebUIRunning() const noexcept { return m_isGunicornRunning; }
  NODISCARD auto isTHREDDSRunning() const noexcept { return m_isApacheRunning; }

private:
  void startServers();
  void startWebServer();
  void startTHREDDS();

  void stopServers();
  void stopWebServer();
  void stopTHREDDS();

  void setEnvironment();

  QProcess m_apacheProcess{this}, m_gunicornProcess{this};
  bool m_isGunicornRunning{false}; ///< gUnicorn server
  bool m_isApacheRunning{false};   ///< Apache tomcat server
  qint64 m_gunicornPID{0};         ///< PID of gUnicorn process
  qint64 m_apachePID{0};           ///< PID of apache tomcat process
};

#endif // SERVERMANAGER_H
