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

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

	void showStatusBarMessage(const char* text) const;

	void checkRemoteConnection();

	void showProgressBar(const char* labelText);
	void updateProgressBar(const int value);
	void hideProgressBar();

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	// Menu callbacks
	void on_actionAbout_Qt_triggered();
	void on_actionClose_triggered();
	void on_actionPreferences_triggered();
	void on_actionAbout_triggered();

	// Current tab changed
	void on_tabWidget_currentChanged(int index);

	void on_actionCheck_for_Updates_triggered();

private:
	//
	void initWidgets();
	//
	void readSettings();
	//
	void writeSettings();
	//
	void setDataOrderRegion();
	//
	void setInitialLayout();
	//
	void setOnline();
	//
	void setOffline();
	//
	void checkForUpdates();
	//
	void showFirstRunConfiguration();

	Ui::MainWindow* m_ui{nullptr};
	QPointer<WidgetDashboard> m_widgetDashboard;
	QPointer<WidgetConfigEditor> m_widgetConfigEditor;
	QPointer<WidgetDataOrder> m_widgetDataOrder;

	QSettings m_settings{"Fisheries and Oceans Canada", "Navigator2Go"};

	QTimer m_uplinkTimer{this};
	bool m_hasRemoteUplink{true};

	bool m_firstRun{false};

	Preferences m_prefs;
};

#endif // MAINWINDOW_H
