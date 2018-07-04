#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QEasyDownloader.hpp"
#include "datadownloaddesc.h"
#include "preferences.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QHash>
#include <QPointer>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}

class QListWidgetItem;
class QObject;
class WidgetDashboard;
class WidgetConfigEditor;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

	void showStatusBarMessage(const char* text) const;

	void checkRemoteConnection();

	void updateRemoteDatasetList();

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	// Menu callbacks
	void on_actionAbout_Qt_triggered();
	void on_actionClose_triggered();
	void on_actionPreferences_triggered();
	void on_actionAbout_triggered();

	// List item is double clicked
	void on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem *item);

	// Current tab changed
	void on_tabWidget_currentChanged(int index);

	// Update Dory dataset list
	void on_pushButtonUpdateDoryList_clicked();

	// Download data in queue
	void on_pushButtonDownload_clicked();

	// Updates THREDDS aggregate files
	// and datasetconfigOFFLINE
	void on_pushButtonUpdateAggConfig_clicked();

	void on_actionCheck_for_Updates_triggered();

private:
	//
	void readSettings();
	//
	void writeSettings() const;
	//
	void configureNetwork();
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

	QTimer m_uplinkTimer{this};
	bool m_hasRemoteUplink{true};

	bool m_firstRun{false};

	Preferences m_prefs;

	// Network stuff
	QNetworkAccessManager m_networkAccessManager{this};
	QEasyDownloader m_downloader{this, &m_networkAccessManager};

	// Stores the resulting JSON objects for each dataset
	// returned by a call to:
	// http://navigator.oceansdata.ca/api/datasets/
	QHash<QString, QJsonObject> m_datasetsAPIResultCache;

	QHash<QString, DataDownloadDesc> m_downloadQueue;

	std::size_t m_numDownloadsComplete{0};
};

#endif // MAINWINDOW_H
