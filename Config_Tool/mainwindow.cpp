#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogdatasetview.h"
#include "dialogpreferences.h"
#include "network.h"
#include "jsonio.h"
#include "process.h"
#include "filecopyworker.h"
#include "ioutils.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QJsonArray>
#include <QSettings>
#include <QNetworkReply>
#include <QProcess>
#include <QThread>
#include <QThreadPool>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
// Timeout for status bar messages
const constexpr int STATUS_BAR_MSG_TIMEOUT{2000};

/***********************************************************************************/
// Colours for server status labels
const constexpr auto COLOR_GREEN{"color: rgb(115, 210, 22);"};
const constexpr auto COLOR_RED{"color: rgb(239, 41, 41);"};

/***********************************************************************************/
enum UITabs : int {
	HOME = 0,
	DATA_SYNC
};

/***********************************************************************************/
MainWindow::MainWindow(QWidget* parent) : 	QMainWindow{parent},
											m_ui{new Ui::MainWindow} {
	m_ui->setupUi(this);
	// Set dark stylesheet
	QFile f{":qdarkstyle/style.qss"};
	if (!f.exists()) {
#ifdef QT_DEBUG
		qDebug() << "Unable to set stylesheet: " << f.errorString();
#endif
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts{&f};
		qApp->setStyleSheet(ts.readAll());
	}

	readSettings();

	configureNetwork();

	checkRemoteConnection();

	m_uplinkTimer.setInterval(300000); // Check for remote uplink every 5 minutes.
	QObject::connect(&m_uplinkTimer, &QTimer::timeout, this, &MainWindow::checkRemoteConnection);
	m_uplinkTimer.start();

	if (m_firstRun) {
		qDebug() << "First run";
	}

	checkForUpdates();

	setDefaultConfigFile();
	updateConfigTargetUI();

	if (m_prefs.UpdateRemoteListOnStart) {
		updateRemoteDatasetList();
	}
	updateActiveDatasetListWidget();

	checkAndStartServers();

	setWindowTitle(tr("Navigator2Go - Beta Build"));

	setInitialLayout();
}

/***********************************************************************************/
MainWindow::~MainWindow() {
	delete m_ui;
}

/***********************************************************************************/
void MainWindow::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this);
}

/***********************************************************************************/
void MainWindow::on_actionClose_triggered() {

	if (m_hasUnsavedData) {
		const auto reply = QMessageBox::question(this, tr("Confirm Action"), tr("Close without saving?"),
										QMessageBox::Yes | QMessageBox::Save | QMessageBox::Cancel);

		switch (reply) {
		case QMessageBox::Save:
			on_pushButtonSaveConfigFile_clicked();
			break;
		case QMessageBox::Cancel:
			return;
		case QMessageBox::Yes:
		default:
			break;
		}
	}

	close();
}

/***********************************************************************************/
void MainWindow::on_buttonAddDataset_clicked() {
	m_hasUnsavedData = true;
	m_ui->pushButtonSaveConfigFile->setEnabled(true);

	m_ui->listWidgetActiveDatasets->addItem("new_dataset_" + QString::number(qrand()));
}

/***********************************************************************************/
void MainWindow::on_listWidgetActiveDatasets_itemDoubleClicked(QListWidgetItem* item) {

	DialogDatasetView dialog{this};
	const auto datasetKey = item->text();
	dialog.SetData(datasetKey, m_documentRootObject[datasetKey].toObject());

	if (dialog.exec()) {
		const auto data = dialog.GetData();

		m_documentRootObject[datasetKey] = data.second;
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonDeleteDataset_clicked() {
	const auto items = m_ui->listWidgetActiveDatasets->selectedItems();

	if (!items.empty())	{
		const auto reply = QMessageBox::question(this, tr("Confirm Action"), tr("Delete selected dataset(s)?"),
												 QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			for (auto* item : items) {
				delete m_ui->listWidgetActiveDatasets->takeItem(m_ui->listWidgetActiveDatasets->row(item));
			}
		}
	}

	m_hasUnsavedData = true;
	m_ui->pushButtonSaveConfigFile->setEnabled(true);
}

/***********************************************************************************/
void MainWindow::closeEvent(QCloseEvent* event) {
	writeSettings();
}

/***********************************************************************************/
void MainWindow::readSettings() {
	QSettings settings{"Fisheries and Oceans Canada", "Navigator2Go"};

	// General
	settings.beginGroup("General");

	if (!settings.contains("FirstRun")) {
		m_firstRun = true;
	}

	if (settings.contains("ONInstallDir")) {
		m_prefs.ONInstallDir = settings.value("ONInstallDir").toString();
	}
	else {
		m_prefs.ONInstallDir = "/opt/Ocean-Data-Map-Project/";
	}

	if (settings.contains("RemoteURL")) {
		m_prefs.RemoteURL = settings.value("RemoteURL").toString();
	}
	else {
		m_prefs.RemoteURL = "http://www.navigator.oceansdata.ca/";
	}

	if (settings.contains("THREDDSDataLocation")) {
		m_prefs.THREDDSDataLocation = settings.value("THREDDSDataLocation").toString();
	}
	else {
		m_prefs.THREDDSDataLocation = "";
	}

	if (settings.contains("UpdateRemoteListOnStart")) {
		m_prefs.UpdateRemoteListOnStart = settings.value("UpdateRemoteListOnStart").toBool();
	}
	else {
		m_prefs.UpdateRemoteListOnStart = true;
	}

	if (settings.contains("AutoStartServers")) {
		m_prefs.AutoStartServers = settings.value("AutoStartServers").toBool();
	}
	else {
		m_prefs.AutoStartServers = false;
	}

	if (settings.contains("IsOnline")) {
		m_prefs.IsOnline = settings.value("IsOnline").toBool();
	}
	else {
		m_prefs.IsOnline = false;
	}

	settings.endGroup();

	// DataOrder
	settings.beginGroup("DataOrder");

	// If the settings has MinLat, it has all the points
	if (settings.contains("MinLat")) {
		m_ui->spinboxMinLat->setValue(settings.value("MinLat").toDouble());
		m_ui->spinboxMaxLat->setValue(settings.value("MaxLat").toDouble());
		m_ui->spinboxMinLon->setValue(settings.value("MinLon").toDouble());
		m_ui->spinboxMaxLon->setValue(settings.value("MaxLon").toDouble());
	}

	settings.endGroup();
}

/***********************************************************************************/
void MainWindow::writeSettings() const {
	QSettings settings{"Fisheries and Oceans Canada", "Navigator2Go"};

	// General
	settings.beginGroup("General");

	settings.setValue("FirstRun", false);
	settings.setValue("ONInstallDir", m_prefs.ONInstallDir);
	settings.setValue("RemoteURL", m_prefs.RemoteURL);
	settings.setValue("THREDDSDataLocation", m_prefs.THREDDSDataLocation);
	settings.setValue("UpdateRemoteListOnStart", m_prefs.UpdateRemoteListOnStart);
	settings.setValue("AutoStartServers", m_prefs.AutoStartServers);
	settings.setValue("IsOnline", m_prefs.IsOnline);

	settings.endGroup();

	// DataOrder
	settings.beginGroup("DataOrder");

	settings.setValue("MinLat", m_ui->spinboxMinLat->value());
	settings.setValue("MaxLat", m_ui->spinboxMaxLat->value());
	settings.setValue("MinLon", m_ui->spinboxMinLon->value());
	settings.setValue("MaxLon", m_ui->spinboxMaxLon->value());

	settings.endGroup();
}

/***********************************************************************************/
void MainWindow::configureNetwork() {
	// Follow server redirects for same domain only
	m_networkManager.setRedirectPolicy(QNetworkRequest::RedirectPolicy::SameOriginRedirectPolicy);

	// Configure downloader
#ifdef QT_DEBUG
	m_downloader.setDebug(true);
	QObject::connect(&m_downloader, &QEasyDownloader::Debugger, this,
					 [&](const auto& msg) {
						qDebug() << msg;
						return;
					}
	);
#endif

	// Full Download Progress. Emitted on every download.
	QObject::connect(&m_downloader, &QEasyDownloader::DownloadProgress, this,
					 [&](const auto bytesReceived, const auto percent, const auto speed, const auto& unit, const auto& url, const auto& filename) {
						qDebug() << percent;
						qDebug() << speed;
					}
	);

	// Emitted when a single file is downloaded.
	QObject::connect(&m_downloader, &QEasyDownloader::DownloadFinished, this,
					 [&](const auto& url, const auto& filename) {
#ifdef QT_DEBUG
						qDebug() << "Downloaded: " << url;
#endif
						this->statusBar()->showMessage(tr(" download complete."), STATUS_BAR_MSG_TIMEOUT);

						++this->m_numDownloadsComplete;
						const auto percent{ 100 * (this->m_numDownloadsComplete / static_cast<std::size_t>(this->m_downloadQueue.size())) };
						this->m_ui->progressBarDownload->setValue(static_cast<int>(percent));
					}
	);

	// Emitted when all jobs are done.
	QObject::connect(&m_downloader, &QEasyDownloader::Finished, this,
					 [&]() {
#ifdef QT_DEBUG
						qDebug() << "All downloads complete";
#endif
						m_ui->pushButtonUpdateAggConfig->setEnabled(true);
						m_ui->listWidgetDownloadQueue->clear();
						m_ui->progressBarDownload->setVisible(false);

						m_downloadQueue.clear();

						QMessageBox box{this};
						box.setWindowTitle(tr("Downloads completed..."));
						box.setText(tr("All downloads completed! Your queue has been emptied."));
						box.setInformativeText(tr("Be sure to click the button below to update your config and aggregation files!"));
						box.setIcon(QMessageBox::Information);

						box.exec();
					}
	);

	// Emitted on error.
	QObject::connect(&m_downloader, &QEasyDownloader::Error, this,
					 [&](const auto& errorCode, const auto& url, const auto& filename) {
#ifdef QT_DEBUG
						qDebug() << "Error: " << errorCode << " " << url;
#endif
						++this->m_numDownloadsComplete;
						const auto percent = 100 * (this->m_numDownloadsComplete / static_cast<std::size_t>(this->m_downloadQueue.size()));
						this->m_ui->progressBarDownload->setValue(static_cast<int>(percent));

						QMessageBox box{this};
						box.setIcon(QMessageBox::Critical);
						box.setWindowTitle(tr("Download error..."));
						box.setText(tr("A error has occoured while downloading your file. If it exists, we will continue with the next file in your queue."));
						box.setDetailedText( url.toString() + "\n\n QNetworkReply::NetworkError: " + QVariant(errorCode).toString() +
											" http://doc.qt.io/archives/qt-4.8/qnetworkreply.html#NetworkError-enum");

						box.exec();

						// Move on to next file in queue.
						if (m_downloader.HasNext()) {
#ifdef QT_DEBUG
							qDebug() << "Moving on to next file";
#endif
							m_downloader.Next();
						}
						else {
							m_ui->pushButtonUpdateAggConfig->setEnabled(true);
							m_ui->listWidgetDownloadQueue->clear();
							m_ui->progressBarDownload->setVisible(false);

							m_downloadQueue.clear();
						}
					}
	);

	// Emitted when there is a timeout.
	QObject::connect(&m_downloader, &QEasyDownloader::Timeout, this,
					 [&](const auto& url, const auto& filename) {
						QMessageBox box{this};
						box.setWindowTitle(tr("Download has timed out..."));
						box.setInformativeText("URL: " + url.toString());
						box.setDetailedText(filename);

						box.setIcon(QMessageBox::Warning);

						box.exec();
					}
	);

}

/***********************************************************************************/
void MainWindow::updateRemoteDatasetList() {
	statusBar()->showMessage(tr("Updating remote dataset list..."), STATUS_BAR_MSG_TIMEOUT);

	const std::function<void(QJsonDocument)> replyHandler = [&](const auto& doc) {
		const auto root = doc.array();

		m_ui->listWidgetDoryDatasets->clear();
		m_datasetsAPIResultCache.clear();
		for (const auto& dataset : root) {
			const auto valueString = dataset["value"].toString();
			m_ui->listWidgetDoryDatasets->addItem(valueString);

			m_datasetsAPIResultCache.insert(valueString, dataset.toObject());
		}

		m_ui->pushButtonUpdateDoryList->setEnabled(true);
		m_ui->pushButtonUpdateDoryList->setText(tr("Update List"));

		this->statusBar()->showMessage(tr("Remote dataset list updated."), STATUS_BAR_MSG_TIMEOUT);
	};

	const std::function<void()> errorHandler = [&]() {
		m_ui->pushButtonUpdateDoryList->setEnabled(true);
		m_ui->pushButtonUpdateDoryList->setText(tr("Update List"));

		this->statusBar()->showMessage(tr("Failed to update remote dataset list."), STATUS_BAR_MSG_TIMEOUT);
	};

	m_ui->pushButtonUpdateDoryList->setEnabled(false);
	m_ui->pushButtonUpdateDoryList->setText(tr("Updating..."));

	Network::MakeAPIRequest(m_networkManager, m_prefs.RemoteURL+"/api/datasets/", replyHandler, errorHandler);
}

/***********************************************************************************/
void MainWindow::updateActiveDatasetListWidget() {

	m_ui->listWidgetActiveDatasets->clear();

	for (const auto& datasetName : m_documentRootObject.keys()) {
		m_ui->listWidgetActiveDatasets->addItem(datasetName);
	}

	m_ui->labelActiveConfigFile->setVisible(true);
	m_ui->labelActiveConfigFile->setText(tr("Active Config File: ") + QFileInfo(m_activeConfigFile).fileName());
	m_ui->buttonAddDataset->setEnabled(true);
	m_ui->pushButtonDeleteDataset->setEnabled(true);
}

/***********************************************************************************/
void MainWindow::on_actionPreferences_triggered() {
	DialogPreferences prefsDialog{this};

	prefsDialog.SetPreferences(m_prefs);

	if (prefsDialog.exec()) {
		// Store previous network state
		const auto prevIsOnline{ m_prefs.IsOnline };
		m_prefs = prefsDialog.GetPreferences();

		setDefaultConfigFile();

		updateActiveDatasetListWidget();

		updateConfigTargetUI();

		// Show a notification to restart gUnicorn
		// is network status changed
		if (m_prefs.IsOnline != prevIsOnline) {
			QMessageBox box{this};
			box.setWindowTitle(tr("Online status changed..."));
			box.setText(tr("You've changed the network status of Navigator2Go.\
							Please Stop and Start the Web Server in the Dashboard tab."));
			box.setIcon(QMessageBox::Icon::Information);

			box.exec();
		}
	}
}

/***********************************************************************************/
void MainWindow::on_actionAbout_triggered() {
	QMessageBox::information(this, tr("About Navigator2Go"), tr(""));
}

/***********************************************************************************/
void MainWindow::on_tabWidget_currentChanged(int index) {

	switch (index) {
	case UITabs::HOME:
		break;
	case UITabs::DATA_SYNC:
		break;
	default:
		break;
	}

}

/***********************************************************************************/
void MainWindow::on_pushButtonUpdateDoryList_clicked() {
	updateRemoteDatasetList();
}

/***********************************************************************************/
void MainWindow::on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item) {
	const auto datasetID{m_datasetsAPIResultCache[item->text()]};
	DialogDatasetView dialog{this};

	auto isUpdatingDownload{false};
	if (m_downloadQueue.find(item->text()) != m_downloadQueue.end()) {
		isUpdatingDownload = true;
	}

	dialog.SetData(datasetID, m_networkManager);

	if (dialog.exec()) {
		const auto data = dialog.GetDownloadData();
		// Only add to queue if variables were selected.
		if (!data.SelectedVariables.empty()) {

			// Don't accept a giant date range
			std::size_t dayLimit = 60;
			if (m_datasetsAPIResultCache[data.ID]["quantum"] == "month") {
				dayLimit = 1825; // 5 years of monthly data only
			}

			if (static_cast<std::size_t>(data.StartDate.daysTo(data.EndDate)) > dayLimit) {
				QMessageBox box{this};
				box.setWindowTitle(tr("Selected date range was too large..."));
				box.setText(tr("For datasets with quantum \"day\" and \"hour\", 60 days is the limit. \nFor datasets with quantum \"month\", 5 years is the limit."));
				box.setIcon(QMessageBox::Critical);
				box.setStandardButtons(QMessageBox::StandardButton::Ok);

				box.exec();

				return;
			}

			if (!isUpdatingDownload) {
				m_ui->listWidgetDownloadQueue->addItem(data.Name);
			}
			m_downloadQueue.insert(data.Name, data);
		}
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonDownload_clicked() {
	m_ui->listWidgetDownloadQueue->selectAll();
	const auto items{ m_ui->listWidgetDownloadQueue->selectedItems() };

	if (items.empty()) {
		QMessageBox::information(this, tr("Download queue empty"), tr("Your download queue is empty! Add some stuff to download!"));
		return;
	}

	if (m_prefs.THREDDSDataLocation.isEmpty()) {
		QMessageBox::information(this, tr("THREDDS location not set..."), tr("Go to Preferences (CTRL + SHIFT + P) to set the location on disk."));
		return;
	}

	QMessageBox box{this};
	box.setWindowTitle(tr("Confirm Download?"));
	box.setText(tr("Depending on your network connection speed, the download could take a significant period of time to complete."));
	box.setIcon(QMessageBox::Question);
	box.setStandardButtons(QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok);

	if (box.exec() == QMessageBox::StandardButton::Ok) {
		// Disable buttons
		m_ui->pushButtonUpdateDoryList->setEnabled(false);
		m_ui->pushButtonDownload->setEnabled(false);

		// Get given region of interest
		const QString min_range{ "&min_range=" + QString::number(m_ui->spinboxMinLat->value()) + "," + QString::number(m_ui->spinboxMinLon->value()) };
		const QString max_range{ "&max_range=" + QString::number(m_ui->spinboxMaxLat->value()) + "," + QString::number(m_ui->spinboxMaxLon->value()) };

		for (const auto& item : m_downloadQueue) {
			const auto url{ item.ToAPIURL() + min_range + max_range };

			m_downloader.Download(url, "/home/nabil/download.nc");
		}

		// Show download stuff
		m_ui->labelDownloadProgress->setVisible(true);
		m_ui->progressBarDownload->setVisible(true);
		m_ui->progressBarDownload->setValue(0);
	}
}

/***********************************************************************************/
void MainWindow::setInitialLayout() {
	m_ui->buttonAddDataset->setText(tr("Add Dataset"));
	m_ui->pushButtonDeleteDataset->setText(tr("Delete Dataset"));
	m_ui->tabWidget->setCurrentIndex(UITabs::HOME);

	m_ui->labelDownloadProgress->setVisible(false);
	m_ui->progressBarDownload->setVisible(false);

	m_ui->labelDatasetTarget->setStyleSheet(COLOR_GREEN);

	m_ui->labelUpdate->setStyleSheet(COLOR_GREEN);
	m_ui->labelUpdate->setVisible(false);
	m_ui->pushButtonUpdate->setEnabled(false);
	m_ui->pushButtonUpdate->setVisible(false);
}

/***********************************************************************************/
void MainWindow::on_pushButtonStartWebServer_clicked() {
	if (!m_gunicornRunning) {
		QProcess process{this};
		process.setProgram("/bin/sh");
		process.setWorkingDirectory(m_prefs.ONInstallDir);
		process.setArguments({"runserver.sh", QFileInfo("datasetconfigONLINE.json").fileName()});

		if (!process.startDetached(nullptr)) {
			QMessageBox box{this};
			box.setWindowTitle(tr("Error"));
			box.setText(tr("Failed to start gUnicorn."));
			box.setDetailedText(tr("Check the permissions of the /tmp/oceannavigator folder."));
			box.setIcon(QMessageBox::Critical);

			box.exec();

			return;
		}

		m_ui->labelStatusWebServer->setText(tr("Running"));
		m_ui->labelStatusWebServer->setStyleSheet(COLOR_GREEN);
		m_gunicornRunning = true;
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonStopWebServer_clicked() {
	if (m_gunicornRunning) {
		if (!QProcess::startDetached("pkill", {"gunicorn"})) {
			QMessageBox box{this};
			box.setWindowTitle(tr("Error"));
			box.setText(tr("Failed to kill gUnicorn process. It might already be stopped. Use ps -aux | grep gunicorn to verify."));
			box.setIcon(QMessageBox::Critical);

			box.exec();

			return;
		}
		m_ui->labelStatusWebServer->setText(tr("Stopped"));
		m_ui->labelStatusWebServer->setStyleSheet(COLOR_RED);
		m_gunicornRunning = false;
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonStartApache_clicked() {
	if (!m_apacheRunning) {
		QProcess process{this};
		process.setProgram("/bin/sh");
		process.setWorkingDirectory(IO::TOMCAT_BIN_DIR);
		process.setArguments({"startup.sh"});

		if (!process.startDetached()) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to start THREDDS server."));

			return;
		}

		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(COLOR_GREEN);
		m_apacheRunning = true;
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonStopApache_clicked() {
	if (m_apacheRunning) {
		QProcess process{this};
		process.setProgram("/bin/sh");
		process.setWorkingDirectory(IO::TOMCAT_BIN_DIR);
		process.setArguments({"shutdown.sh"});

		if (!process.startDetached()) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to stop THREDDS server."));

			return;
		}

		m_ui->labelStatusApache->setText(tr("Stopped"));
		m_ui->labelStatusApache->setStyleSheet(COLOR_RED);
		m_apacheRunning = false;
	}
}

/***********************************************************************************/
void MainWindow::checkAndStartServers() {
	// gUnicorn
	m_gunicornRunning = IsProcessRunning("gunicorn");
	if (!m_gunicornRunning && m_prefs.AutoStartServers) {
		on_pushButtonStartWebServer_clicked();
		m_gunicornRunning = true;
	}
	if (m_gunicornRunning) {
		m_ui->labelStatusWebServer->setText(tr("Running"));
		m_ui->labelStatusWebServer->setStyleSheet(COLOR_GREEN);
	}

	// Apache tomcat
	m_apacheRunning = IsProcessRunning("java");
	if (!m_apacheRunning && m_prefs.AutoStartServers) {
		on_pushButtonStartApache_clicked();
		m_apacheRunning = true;
	}
	if (m_apacheRunning) {
		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(COLOR_GREEN);
	}
}

/***********************************************************************************/
void MainWindow::updateConfigTargetUI() {
	if (m_prefs.IsOnline) {
		m_ui->labelDatasetTarget->setText(tr("Remote Storage: \n") + m_prefs.RemoteURL);
	}
	else {
		m_ui->labelDatasetTarget->setText(tr("Local Storage"));
	}
}

/***********************************************************************************/
void MainWindow::setDefaultConfigFile() {
	const static auto onlineConfig{ m_prefs.ONInstallDir+"/oceannavigator/datasetconfigONLINE.json" };
	const static auto offlineConfig{ m_prefs.ONInstallDir+"/oceannavigator/datasetconfigOFFLINE.json" };

	QString newConfigFile;
	if (m_prefs.IsOnline) {
		newConfigFile = onlineConfig;
	}
	else {
		newConfigFile = offlineConfig;
	}

	// Validate file
	auto doc = IO::LoadJSONFile(newConfigFile);
	if (doc.isNull()) {
#ifdef QT_DEBUG
		qDebug() << "Config file not found: " << newConfigFile;
#endif
		QMessageBox box{this};
		box.setWindowTitle(tr("File not found..."));
		box.setText(tr("No default config file was found. Do you want to create one at: ")
					+ m_prefs.ONInstallDir+"/oceannavigator/"+newConfigFile);
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		box.setIcon(QMessageBox::Question);

		if (box.exec() != QMessageBox::Yes) {
			return;
		}
#ifdef QT_DEBUG
		qDebug() << "Creating it...";
#endif
		// File doesn't exist so create it
		IO::WriteJSONFile(newConfigFile);

		// And now load it
		doc = IO::LoadJSONFile(newConfigFile);

	}

	m_activeConfigFile = newConfigFile;
	m_documentRootObject = doc.object();

	statusBar()->showMessage(tr("Config file loaded: ") + m_activeConfigFile, STATUS_BAR_MSG_TIMEOUT);
}

/***********************************************************************************/
void MainWindow::setCustomConfigFile(const QString& filePath) {

	// Validate file
	const auto doc = IO::LoadJSONFile(filePath);

	if (doc.isNull()) {
		return;
	}

	m_activeConfigFile = filePath;
	m_documentRootObject = doc.object();

	statusBar()->showMessage(tr("Config file loaded: ") + m_activeConfigFile, STATUS_BAR_MSG_TIMEOUT);
}

/***********************************************************************************/
void MainWindow::saveConfigFile() {
	const QJsonDocument doc{m_documentRootObject};

	IO::WriteJSONFile(
		#ifdef QT_DEBUG
				"/home/nabil/dory.json"
		#else
				m_activeConfigFile
		#endif
				, m_documentRootObject);

	statusBar()->showMessage(tr("Config file saved: ") + m_activeConfigFile, STATUS_BAR_MSG_TIMEOUT);

	m_hasUnsavedData = false;
	m_ui->pushButtonSaveConfigFile->setEnabled(false);
}

/***********************************************************************************/
int MainWindow::showUnsavedDataMessageBox() {

	return QMessageBox::question(this,
								 tr("Confirm continue..."),
								 tr("You have unsaved changes. Do you want to save them?"),
								 QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel
								 );
}

/***********************************************************************************/
void MainWindow::checkForUpdates() {
}

/***********************************************************************************/
void MainWindow::checkRemoteConnection() {
	// QThreadPool deletes automatically
	auto* task{ new Network::URLExistsRunnable{m_prefs.RemoteURL} };

	// Setup connection
	QObject::connect(task, &Network::URLExistsRunnable::urlResult, this, [&](const auto success) {
		QMessageBox box{this};

		if (success) {
			if (!m_hasRemoteUplink) {
				m_hasRemoteUplink = true;

				box.setWindowTitle(tr("Remote uplink restored..."));
				box.setIcon(QMessageBox::Information);
				box.setText(tr("You have restored connection to the remote server. Nice job! Would you like to switch to your remote server's datasets?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

				if (box.exec() == QMessageBox::Yes) {
					this->setOnline();
				}
			}

			this->m_ui->labelRemoteUplink->setText(tr("Online"));
			this->m_ui->labelRemoteUplink->setStyleSheet(COLOR_GREEN);
			this->statusBar()->showMessage(tr("Remote uplink test successful."), STATUS_BAR_MSG_TIMEOUT);
			this->updateRemoteDatasetList();
		}
		else {
			if (m_hasRemoteUplink) {
				m_hasRemoteUplink = false;

				box.setWindowTitle(tr("Remote uplink lost..."));
				box.setIcon(QMessageBox::Warning);
				box.setText(tr("You have lost connection to the remote server. Would you like to switch to your local datasets?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

				if (box.exec() == QMessageBox::Yes) {
					this->setOffline();
				}
			}

			this->m_ui->labelRemoteUplink->setText(tr("Offline"));
			this->m_ui->labelRemoteUplink->setStyleSheet(COLOR_RED);
			this->statusBar()->showMessage(tr("Remote uplink test failed"), STATUS_BAR_MSG_TIMEOUT);
		}

		m_ui->pushButtonCheckRemoteUplink->setEnabled(true);
		m_ui->pushButtonCheckRemoteUplink->setText(tr("Test"));

	}, Qt::BlockingQueuedConnection); // <-- Check out this magic...this would segfault otherwise

	m_ui->pushButtonCheckRemoteUplink->setEnabled(false);
	m_ui->pushButtonCheckRemoteUplink->setText(tr("Checking..."));

	QThreadPool::globalInstance()->start(task);
}

/***********************************************************************************/
void MainWindow::setOnline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to online.";
#endif
	m_prefs.IsOnline = true;
	setDefaultConfigFile();
	updateConfigTargetUI();
}

/***********************************************************************************/
void MainWindow::setOffline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to offline.";
#endif
	m_prefs.IsOnline = false;
	setDefaultConfigFile();
	updateConfigTargetUI();
}

/***********************************************************************************/
void MainWindow::on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem* item) {
	delete m_ui->listWidgetDownloadQueue->takeItem(m_ui->listWidgetDownloadQueue->row(item));
}

/***********************************************************************************/
void MainWindow::on_pushButtonUpdateAggConfig_clicked() {
	m_ui->pushButtonUpdateAggConfig->setEnabled(false);
}

/***********************************************************************************/
void MainWindow::on_pushButtonSaveConfigFile_clicked() {
	saveConfigFile();
}

/***********************************************************************************/
void MainWindow::on_pushButtonImportNetCDF_clicked() {
	const auto files = QFileDialog::getOpenFileNames(this,
													 tr("Select NetCDF files to import..."),
													 QDir::currentPath(),
													 "NetCDF Files (*.nc)"
													 );
	if (files.empty()) {
		return;
	}

	m_workerThread = new QThread;
	auto* worker{ new IO::FileCopyWorker(files) };
	worker->moveToThread(m_workerThread);

	QObject::connect(worker, &IO::FileCopyWorker::error, this, [&](const auto& error) {
#ifdef QT_DEBUG
		qDebug() << error;
#endif
		QMessageBox::critical(this, tr("File copy error..."), error);
	});

	QObject::connect(m_workerThread, &QThread::started, worker, &IO::FileCopyWorker::copyFiles);
	// Resource clean-up connections
	QObject::connect(worker, &IO::FileCopyWorker::finished, m_workerThread, &QThread::quit);
	QObject::connect(worker, &IO::FileCopyWorker::finished, worker, &IO::FileCopyWorker::deleteLater);
	QObject::connect(m_workerThread, &QThread::finished, m_workerThread, &QThread::deleteLater);

	// Run it
	m_workerThread->start(QThread::HighestPriority);
}

/***********************************************************************************/
void MainWindow::on_pushButtonLoadCustomConfig_clicked() {
	if (m_hasUnsavedData) {
		switch(showUnsavedDataMessageBox()) {
		case QMessageBox::Yes:
			saveConfigFile();
			break;
		case QMessageBox::No:
			break;
		case QMessageBox::Cancel:
			return;
		}
	}

	const auto filePath{ QFileDialog::getOpenFileName(this,
													   tr("Select a dataset config file..."),
													   QDir::currentPath(),
													   "Config Files (*.json)"
													   )
					   };
	if (filePath.isNull()) {
		return;
	}

	setCustomConfigFile(filePath);
	updateActiveDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::on_pushButtonLoadDefaultConfig_clicked() {
	if (m_hasUnsavedData) {
		switch(showUnsavedDataMessageBox()) {
		case QMessageBox::Yes:
			saveConfigFile();
			break;
		case QMessageBox::No:
			break;
		case QMessageBox::Cancel:
			return;
		}
	}

	setDefaultConfigFile();
	updateConfigTargetUI();
	updateActiveDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::on_actionCheck_for_Updates_triggered() {
	checkForUpdates();
}

/***********************************************************************************/
void MainWindow::on_pushButtonUpdate_clicked() {
	checkForUpdates();
}

/***********************************************************************************/
void MainWindow::on_pushButtonCheckRemoteUplink_clicked() {
	checkRemoteConnection();
}
