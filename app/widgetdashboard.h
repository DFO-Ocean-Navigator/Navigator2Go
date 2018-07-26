#ifndef DASHBOARDFORM_H
#define DASHBOARDFORM_H

#include <QWidget>
#include <QProcess>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetDashboard;
}

class MainWindow;
struct Preferences;

/***********************************************************************************/
class WidgetDashboard : public QWidget {
	Q_OBJECT

public:
	WidgetDashboard(QWidget* parent, MainWindow* mainWindow, const Preferences* prefs);

	~WidgetDashboard();

	void showOnlineText();
	void showOfflineText();

	void enableUplinkTestButton();
	void disableUplinkTestButton();

private slots:
	void on_pushButtonStartWebServer_clicked(); // gUnicorn
	void on_pushButtonStopWebServer_clicked();
	void on_pushButtonStopApache_clicked();	// Apache Tomcat
	void on_pushButtonStartApache_clicked();

	void on_pushButtonImportNetCDF_clicked();

	void on_pushButtonCheckRemoteUplink_clicked();

	void on_lineEditCommandPrompt_returnPressed();

private:
	//
	void checkAndStartServers();
	//
	void updateDriveInfo();

	Ui::WidgetDashboard* m_ui{nullptr};
	MainWindow* const m_mainWindow{nullptr};

	const Preferences* const m_prefs{nullptr};

	QProcess m_consoleProcess{this};

	// Which servers are running locally
	bool m_isGunicornRunning{false}, m_apacheRunning{false};
};

#endif // DASHBOARDFORM_H
