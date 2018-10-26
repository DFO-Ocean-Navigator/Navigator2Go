#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datadownloaddesc.h"
#include "preferences.h"
#include "servermanager.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QTimer>
#include <QSettings>
#include <QPointer>
#include <QProcess>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}

class WidgetConfigEditor;
class WidgetDataOrder;
class WidgetThreddsConfig;
class WidgetLocalData;
class QNetworkReply;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	void showStatusBarMessage(const char* text) const;
	/// Show the progress bar and label with the given labelText. Starts at 0%
	void showProgressBar(const char* labelText);
	/// Updates the progress bar percent value. Value is a percentage from 0 to 100.
	void updateProgressBar(const int value);
	/// Hide progress bar and label
	void hideProgressBar();

private slots:
	///
	void refreshRequestHandler();

	// Menu callbacks

	/// About Qt
	void on_actionAbout_Qt_triggered();
	/// Program close
	void on_actionClose_triggered();
	/// Preferences
	void on_actionPreferences_triggered();
	/// About
	void on_actionAbout_triggered();
	/// Update check
	void on_actionCheck_for_Updates_triggered();

	void on_actionNavigator2Go_Manual_triggered();

	void on_pushButtonLaunchViewer_clicked();

	void on_pushButtonOpenDataOrder_clicked();

	void on_pushButtonImportNCFiles_clicked();

	void on_pushButtonViewLocalData_clicked();

	void on_actionClear_Python_cache_triggered();

	void on_pushButtonRefresh_clicked();

	void on_tabWidget_currentChanged(int index);

private:
	/// Queries the RemoteURL in m_prefs for a connection
	void checkRemoteConnection();
	///
	void updateTHREDDSConfigTable();
	/// Initialize widgets for each tab
	void initWidgets();
	/// Sets initial layout of program on start
	void setInitialLayout();
	/// Calls both update functions for convenience
	void checkForUpdates();
	/// Checks for updates for Navigator2Go.
	void checkForAppUpdate();
	/// Updates Ocean Navigator code from git and rebuilds necessary code.
	void checkForONUpdates();
	/// Show first run configuration dialog before main window pops up.
	void showFirstRunConfiguration();

	Ui::MainWindow* m_ui{nullptr};
	ServerManager m_serverManager{this};
	QPointer<WidgetLocalData> m_widgetLocalData;
	QPointer<WidgetConfigEditor> m_widgetConfigEditor;
	QPointer<WidgetDataOrder> m_widgetDataOrder;
	QPointer<WidgetThreddsConfig> m_widgetThreddsConfig;
	QPointer<QNetworkReply> m_updateReply;

	Preferences m_prefs{this}; ///< Serializes user settings

	QTimer m_uplinkTimer{this}; ///< Timer to check for a remote navigator connection every 5 minutes.
	bool m_hasRemoteUplink{true}; ///< Does this client have a connection to the remote Navigator server
};

#endif // MAINWINDOW_H
