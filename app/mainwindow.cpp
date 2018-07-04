#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogdatasetview.h"
#include "dialogpreferences.h"
#include "widgetdashboard.h"
#include "widgetconfigeditor.h"

#include "network.h"
#include "jsonio.h"
#include "process.h"
#include "ioutils.h"
#include "defines.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QJsonArray>
#include <QSettings>
#include <QNetworkReply>
#include <QThreadPool>

#include <netcdf4/ncFile.h>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

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

	if (m_firstRun) {
#ifdef QT_DEBUG
		qDebug() << "First run";
#endif
		showFirstRunConfiguration();
	}

	// Init widgets
	m_widgetDashboard = new WidgetDashboard(m_ui->tabWidget, this, &m_prefs);
	m_widgetConfigEditor = new WidgetConfigEditor(m_ui->tabWidget, this, &m_prefs);

	configureNetwork();

	checkRemoteConnection();

	m_uplinkTimer.setInterval(300000); // Check for remote uplink every 5 minutes.
	QObject::connect(&m_uplinkTimer, &QTimer::timeout, this, &MainWindow::checkRemoteConnection);
	m_uplinkTimer.start();


	if (m_prefs.IsNetworkOnline) {
		setOnline();
	} else {
		setOffline();
	}

	checkForUpdates();

	if (m_prefs.UpdateRemoteListOnStart) {
		updateRemoteDatasetList();
	}

	setWindowTitle(tr("Navigator2Go - Alpha Build"));

	setInitialLayout();
}

/***********************************************************************************/
MainWindow::~MainWindow() {
	delete m_ui;
}

/***********************************************************************************/
void MainWindow::showStatusBarMessage(const char* text) const {
	statusBar()->showMessage(tr(text), STATUS_BAR_MSG_TIMEOUT);
}

/***********************************************************************************/
void MainWindow::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this);
}

/***********************************************************************************/
void MainWindow::on_actionClose_triggered() {

	if (m_widgetConfigEditor->hasUnsavedData()) {
		const auto reply{ QMessageBox::question(this, tr("Confirm Action"), tr("Close without saving?"),
										QMessageBox::Yes | QMessageBox::Save | QMessageBox::Cancel) };

		switch (reply) {
		case QMessageBox::Save:
			m_widgetConfigEditor->saveConfigFile();
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

	if (settings.contains("UpdateRemoteListOnStart")) {
		m_prefs.UpdateRemoteListOnStart = settings.value("UpdateRemoteListOnStart").toBool();
	}

	if (settings.contains("AutoStartServers")) {
		m_prefs.AutoStartServers = settings.value("AutoStartServers").toBool();
	}

	if (settings.contains("IsNetworkOnline")) {
		m_prefs.IsNetworkOnline = settings.value("IsNetworkOnline").toBool();
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
	settings.setValue("IsNetworkOnline", m_prefs.IsNetworkOnline);

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
	m_networkAccessManager.setRedirectPolicy(QNetworkRequest::RedirectPolicy::SameOriginRedirectPolicy);

	if (m_prefs.IsNetworkOnline) {
		// Reduce latency by connecting to remote first
		m_networkAccessManager.connectToHost(m_prefs.RemoteURL);
	}

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
#ifdef QT_DEBUG
						qDebug() << percent;
						qDebug() << speed;
#endif
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

						if (percent == 100) {
							// Prevent divide-by-zero if this is in the below lambda
							m_downloadQueue.clear();
						}
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
						m_ui->labelDownloadProgress->setVisible(false);

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

	m_ui->pushButtonUpdateDoryList->setEnabled(false);
	m_ui->pushButtonUpdateDoryList->setText(tr("Updating..."));

	Network::MakeAPIRequest(m_networkAccessManager, m_prefs.RemoteURL+"/api/datasets/",
							// Success handler
							[&](const auto& doc) {
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
							},
							// Error handler
							[&]() {
								m_ui->pushButtonUpdateDoryList->setEnabled(true);
								m_ui->pushButtonUpdateDoryList->setText(tr("Update List"));

								this->statusBar()->showMessage(tr("Failed to update remote dataset list."), STATUS_BAR_MSG_TIMEOUT);
							}
	);
}

/***********************************************************************************/
void MainWindow::on_actionPreferences_triggered() {
	DialogPreferences prefsDialog{this};

	prefsDialog.SetPreferences(m_prefs);

	if (prefsDialog.exec()) {
		// Store previous network state
		const auto prevNetworkState{ m_prefs.IsNetworkOnline };
		m_prefs = prefsDialog.GetPreferences();

		// Has the network state changed?
		if (m_prefs.IsNetworkOnline != prevNetworkState) {

			m_prefs.IsNetworkOnline ? setOnline() : setOffline();

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

	dialog.SetData(datasetID, m_networkAccessManager);

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
#ifdef QT_DEBUG
			qDebug() << IO::FindPathForDataset(item);
#endif

			m_downloader.Download(url, IO::FindPathForDataset(item) + ".nc");
		}

		// Show download stuff
		m_ui->labelDownloadProgress->setVisible(true);
		m_ui->progressBarDownload->setVisible(true);
		m_ui->progressBarDownload->setValue(0);
	}
}

/***********************************************************************************/
void MainWindow::setInitialLayout() {
	m_ui->tabWidget->setCurrentIndex(UITabs::HOME);

	m_ui->labelDownloadProgress->setVisible(false);
	m_ui->progressBarDownload->setVisible(false);

	// Add widgets to their tabs
	m_ui->dashboardLayout->addWidget(m_widgetDashboard);
	m_ui->configEditorLayout->addWidget(m_widgetConfigEditor);
}

/***********************************************************************************/
void MainWindow::checkForUpdates() {
}

/***********************************************************************************/
void MainWindow::setOnline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to online.";
#endif
	m_networkAccessManager.setNetworkAccessible(QNetworkAccessManager::Accessible);
	m_widgetDashboard->showOnlineText();
	m_widgetConfigEditor->setDefaultConfigFile();
	m_widgetConfigEditor->updateDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::setOffline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to offline.";
#endif
	m_networkAccessManager.setNetworkAccessible(QNetworkAccessManager::NotAccessible);
	m_widgetDashboard->showOfflineText();
	m_widgetConfigEditor->setDefaultConfigFile();
	m_widgetConfigEditor->updateDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::checkRemoteConnection() {
	if (m_prefs.IsNetworkOnline) {
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
						m_prefs.IsNetworkOnline = true;
						this->setOnline();
					}
				}

				m_widgetDashboard->showOnlineText();
				this->showStatusBarMessage("Remote uplink test successful.");
				this->updateRemoteDatasetList();
			}
			else {
				if (m_hasRemoteUplink) {
					m_hasRemoteUplink = false;

					box.setWindowTitle(tr("Remote uplink lost..."));
					box.setIcon(QMessageBox::Warning);
					box.setText(tr("You have lost connection to the remote server. Local datasets will now be used."));
					box.setStandardButtons(QMessageBox::Ok);

					m_prefs.IsNetworkOnline = false;

					this->setOffline();
				}

				this->showStatusBarMessage("Remote uplink test failed");
			}

			m_widgetDashboard->enableUplinkTestButton();

		}, Qt::BlockingQueuedConnection); // <-- Check out this magic...this would segfault otherwise

		m_widgetDashboard->disableUplinkTestButton();

		QThreadPool::globalInstance()->start(task);
	}
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
void MainWindow::on_actionCheck_for_Updates_triggered() {
	checkForUpdates();
}

/***********************************************************************************/
void MainWindow::showFirstRunConfiguration() {
	DialogPreferences prefsDialog{this};
	prefsDialog.setWindowTitle(tr("Navigator2Go Initial Setup..."));
	prefsDialog.SetPreferences(m_prefs);

	while(prefsDialog.GetPreferences().THREDDSDataLocation.isEmpty()) {
		prefsDialog.exec();
	}

	m_prefs = prefsDialog.GetPreferences();
}
