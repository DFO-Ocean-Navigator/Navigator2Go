#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datadownloaddesc.h"
#include "preferences.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QTimer>
#include <QSettings>
#include <QPointer>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}

class WidgetDashboard;
class WidgetConfigEditor;
class WidgetDataOrder;
class WidgetThreddsConfig;
class QNetworkReply;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

	void showStatusBarMessage(const char* text) const;
	/// Queries the RemoteURL in m_prefs for a connection
	void checkRemoteConnection();
	/// Show the progress bar and label with the given labelText. Starts at 0%
	void showProgressBar(const char* labelText);
	/// Updates the progress bar percent value. Value is a percentage from 0 to 100.
	void updateProgressBar(const int value);
	/// Hide progress bar and label
	void hideProgressBar();
	///
	void UpdateTHREDDSConfigTable();

protected:
	/// Capture when the program is closing to write settings, cleanup, etc.
	void closeEvent(QCloseEvent* event) override;

private slots:
	// Menu callbacks

	/// About Qt
	void on_actionAbout_Qt_triggered();
	/// Program close
	void on_actionClose_triggered();
	/// Preferences
	void on_actionPreferences_triggered();
	/// About
	void on_actionAbout_triggered();
	/// Current tab changed
	void on_tabWidget_currentChanged(int index);
	/// Update check
	void on_actionCheck_for_Updates_triggered();

private:
	/// Initialize widgets for each tab
	void initWidgets();
	/// Read settings from disk into Preferences object
	void readSettings();
	/// Write settings from Preferences object to disk
	void writeSettings();
	/// Reads the saved data order bounding box from QSettings and sets it in the WidgetDataOrder
	void setDataOrderRegion();
	/// Sets initial layout of program on start
	void setInitialLayout();
	/// Set network state to online
	void setOnline();
	/// Set network state to offline
	void setOffline();
	/// Calls both update functions for convenience
	void checkForUpdates();
	/// Checks for updates for Navigator2Go.
	void checkForAppUpdate();
	/// Updates Ocean Navigator code from git and rebuilds necessary code.
	void checkForONUpdates();
	/// Show first run configuration dialog before main window pops up.
	void showFirstRunConfiguration();

	Ui::MainWindow* m_ui{nullptr};
	QPointer<WidgetDashboard> m_widgetDashboard;
	QPointer<WidgetConfigEditor> m_widgetConfigEditor;
	QPointer<WidgetDataOrder> m_widgetDataOrder;
	QPointer<WidgetThreddsConfig> m_widgetThreddsConfig;
	QPointer<QNetworkReply> m_updateReply;

	QSettings m_settings{"Fisheries and Oceans Canada", "Navigator2Go"}; ///< Holds serialized ini settings. Loaded once into m_settings and saved once from m_settings on program exit.
	Preferences m_prefs; ///< Populated after m_settings is loaded. This is manipulated.

	QTimer m_uplinkTimer{this}; ///< Timer to check for a remote navigator connection every 5 minutes.
	bool m_hasRemoteUplink{true}; ///< Does this client have a connection to the remote Navigator server
};

#endif // MAINWINDOW_H
