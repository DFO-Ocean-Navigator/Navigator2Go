#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QEasyDownloader.hpp"
#include "downloaddata.h"
#include "preferences.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QHash>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}
class QListWidgetItem;
class QObject;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	// Menu callbacks
	void on_actionAbout_Qt_triggered();
	void on_actionClose_triggered();
	void on_actionSave_triggered();
	void on_actionPreferences_triggered();
	void on_actionAbout_triggered();

	// Add Dataset button callback
	void on_buttonAddDataset_clicked();
	// Delete Dataset button callback
	void on_pushButtonDeleteDataset_clicked();

	// List item is double clicked
	void on_listWidgetActiveDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem *item);

	// Current tab changed
	void on_tabWidget_currentChanged(int index);

	void on_pushButtonUpdateDoryList_clicked();
	void on_pushButtonDownload_clicked();

	// Start/Stop servers
	void on_pushButtonStartWebServer_clicked(); // gUnicorn
	void on_pushButtonStopWebServer_clicked();
	void on_pushButtonStopApache_clicked();	// Apache Tomcat
	void on_pushButtonStartApache_clicked();

	void on_pushButtonUpdateAggregate_clicked();

private:
	//
	void readSettings();
	//
	void writeSettings() const;
	//
	void configureNetworkManager();
	//
	void updateDoryDatasetList();
	//
	void updateActiveDatasetListWidget();
	//
	void setInitialLayout();
	//
	void checkAndStartServers();
	//
	void setActiveConfigFile();

	Ui::MainWindow* m_ui{nullptr};

	Preferences m_prefs;

	//
	QString m_activeConfigFile;
	//
	QJsonObject m_documentRootObject;

	// Network stuff
	QNetworkAccessManager m_networkManager{this};
	QEasyDownloader m_downloader{this, &m_networkManager};

	bool m_hasUnsavedData{false};

	// Stores the resulting JSON objects for each dataset
	// returned by a call to:
	// http://navigator.oceansdata.ca/api/datasets/
	QHash<QString, QJsonObject> m_datasetsAPIResultCache;

	QHash<QString, DownloadData> m_downloadQueue;

	// Which servers are running locally
	bool m_gunicornRunning{false}, m_apacheRunning{false};
};

#endif // MAINWINDOW_H
