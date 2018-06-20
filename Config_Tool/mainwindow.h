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
class QThread;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	// Menu callbacks
	void on_actionAbout_Qt_triggered();
	void on_actionClose_triggered();
	void on_actionPreferences_triggered();
	void on_actionAbout_triggered();

	// Add Dataset button callback
	void on_buttonAddDataset_clicked();
	// Delete Dataset button callback
	void on_pushButtonDeleteDataset_clicked();
	// Save button callback
	void on_pushButtonSaveConfigFile_clicked();

	// List item is double clicked
	void on_listWidgetActiveDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem *item);

	// Current tab changed
	void on_tabWidget_currentChanged(int index);

	// Update Dory dataset list
	void on_pushButtonUpdateDoryList_clicked();

	// Download data in queue
	void on_pushButtonDownload_clicked();

	// Start/Stop servers
	void on_pushButtonStartWebServer_clicked(); // gUnicorn
	void on_pushButtonStopWebServer_clicked();
	void on_pushButtonStopApache_clicked();	// Apache Tomcat
	void on_pushButtonStartApache_clicked();

	// Updates THREDDS aggregate files
	// and datasetconfigOFFLINE
	void on_pushButtonUpdateAggConfig_clicked();

	void on_pushButtonImportNetCDF_clicked();

	void on_pushButtonLoadCustomConfig_clicked();

	void on_pushButtonLoadDefaultConfig_clicked();

	void on_actionCheck_for_Updates_triggered();

	void on_pushButtonUpdate_clicked();

	void on_pushButtonCheckDoryUplink_clicked();

private:
	//
	void readSettings();
	//
	void writeSettings() const;
	//
	void configureNetwork();
	//
	void updateDoryDatasetList();
	//
	void updateActiveDatasetListWidget();
	//
	void setInitialLayout();
	//
	void checkAndStartServers();
	//
	void setDefaultConfigFile();
	//
	void setCustomConfigFile(const QString& filePath);
	//
	void saveConfigFile();
	//
	int showUnsavedDataMessageBox();
	//
	void checkForUpdates();
	//
	void checkDoryConnection();
	//
	void setOnline();
	//
	void setOffline();
	//
	void updateConfigTargetUI();

	Ui::MainWindow* m_ui{nullptr};

	QThread* m_workerThread{nullptr};

	QTimer m_uplinkTimer{this};
	bool m_hasDoryUplink{true};

	bool m_firstRun{false};

	Preferences m_prefs;

	// Path to the currently loaded config file
	QString m_activeConfigFile;
	// Root JSON object of active config file
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

	// Location for THREDDS datasets
#ifdef QT_DEBUG
	QString m_threddsRootDatasetLocation{"/home/nabil/"};
#else
	QString m_threddsRootDatasetLocation{""}
#endif

	std::size_t m_numDownloadsComplete{0};

	// Which servers are running locally
	bool m_gunicornRunning{false}, m_apacheRunning{false};
};

#endif // MAINWINDOW_H
