#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogdatasetview.h"
#include "dialogpreferences.h"
#include "apirequest.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QSaveFile>
#include <QSettings>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QNetworkReply>
#include <QProcess>
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
// Checks if a named process is running on a UNIX or Windows system
#ifdef __linux__
auto IsProcessRunning(const QString& processName) {
	static const QString prefix{"ps cax | grep "};
	static const QString postfix{" > /dev/null; if [ $? -eq 0 ]; then echo \"true\"; else echo \"false\"; fi"};

	QProcess process;
	process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());

	auto args = QStringList() << "-c" << prefix + processName + postfix;
	process.start("/bin/sh", args);
	process.waitForFinished();

	// Capture output from bash script
	const QString output =  process.readAll();
	if (output.contains("true", Qt::CaseInsensitive)) {
		return true;
	}

	return false;
}
#elif _WIN32
auto IsProcessRunning(const QString& processName) {
	return false;
}
#endif

/***********************************************************************************/
// Loads a JSON file from disk, checks for errors, and returns the QJsonDocument.
auto LoadJSONFile(const QString& path, const bool showMsgBox = true) {

	// Try to open file
	QFile f{path};
	if (!f.open(QFile::ReadOnly | QFile::Text)) {
#ifdef QT_DEBUG
		qDebug() << "File Open Error: " << path;
		qDebug() << f.errorString();
#endif
		if (showMsgBox) {
			QMessageBox msgBox;
			msgBox.setWindowTitle(QObject::tr("Error..."));
			msgBox.setText(path);
			msgBox.setInformativeText(QObject::tr("File not found!"));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}

		return QJsonDocument();
	}
	const QString contents = f.readAll(); // Get file contents
	f.close();

	// Parse json
	QJsonParseError error; // catch errors
	const auto jsonDocument = QJsonDocument::fromJson(contents.toUtf8(), &error);
	// Check for errors
	if (jsonDocument.isNull()) {
#ifdef QT_DEBUG
		qDebug() << "JSON Error: " << path;
		qDebug() << error.errorString();
#endif
		if (showMsgBox) {
			QMessageBox msgBox;
			msgBox.setWindowTitle(QObject::tr("Error parsing JSON file..."));
			msgBox.setText(path);
			msgBox.setInformativeText(QObject::tr("JSON syntax error detected."));
			msgBox.setDetailedText(error.errorString());
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}

		return QJsonDocument(); // Return empty doc
	}

	return jsonDocument;
}

/***********************************************************************************/
// Writes a JSON file to the given path
void WriteJSONFile(const QString& path, const QJsonObject& obj = QJsonObject()) {

	const QJsonDocument doc{obj};

	QSaveFile f{path};
	if (!f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)); { // Overwrite original file.
		qDebug() << f.errorString();
	}
	f.write(doc.toJson());
	f.commit();
}

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
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}

	readSettings();

	setActiveConfigFile();

	configureNetworkManager();

	if (m_prefs.UpdateDoryListOnStart) {
		updateDoryDatasetList();
	}
	updateActiveDatasetListWidget();

	checkAndStartServers();

	setWindowTitle(tr("Navigator2Go"));

	setInitialLayout();

#ifdef QT_DEBUG
	m_downloader.setDebug(true);
	QObject::connect(&m_downloader, &QEasyDownloader::Debugger,
					 [&](const auto& msg) {
						qDebug() << msg;
						return;
					}
	);
#endif

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
			on_actionSave_triggered();
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
void MainWindow::on_actionSave_triggered() {

	if (!m_activeConfigFile.isEmpty()) {
		const QJsonDocument doc{m_documentRootObject};

		WriteJSONFile(
			#ifdef QT_DEBUG
					"/home/nabil/dory.json"
			#else
					m_activeConfigFile
			#endif
					, m_documentRootObject);

		statusBar()->showMessage(tr("Config file saved!"), STATUS_BAR_MSG_TIMEOUT);

		m_hasUnsavedData = false;
	}
}

/***********************************************************************************/
void MainWindow::on_buttonAddDataset_clicked() {
	m_hasUnsavedData = true;

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
}

/***********************************************************************************/
void MainWindow::closeEvent(QCloseEvent* event) {
	writeSettings();
}

/***********************************************************************************/
void MainWindow::readSettings() {
	QSettings settings{"OceanNavigator", "Config Tool"};

	settings.beginGroup("General");

	if (settings.contains("ONInstallDir")) {
		m_prefs.ONInstallDir = settings.value("ONInstallDir").toString();
	}
	else {
		m_prefs.ONInstallDir = "/opt/tools/Ocean-Data-Map-Project/";
	}

	if (settings.contains("UpdateDoryListOnStart")) {
		m_prefs.UpdateDoryListOnStart = settings.value("UpdateDoryListOnStart").toBool();
	}
	else {
		m_prefs.UpdateDoryListOnStart = true;
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
}

/***********************************************************************************/
void MainWindow::writeSettings() const {
	QSettings settings{"OceanNavigator", "Config Tool"};

	settings.beginGroup("General");

	settings.setValue("ONInstallDir", m_prefs.ONInstallDir);
	settings.setValue("UpdateDoryListOnStart", m_prefs.UpdateDoryListOnStart);
	settings.setValue("AutoStartServers", m_prefs.AutoStartServers);
	settings.setValue("IsOnline", m_prefs.IsOnline);

	settings.endGroup();
}

/***********************************************************************************/
void MainWindow::configureNetworkManager() {
	// Follow server redirects for same domain only
	m_networkManager.setRedirectPolicy(QNetworkRequest::RedirectPolicy::SameOriginRedirectPolicy);
}

/***********************************************************************************/
void MainWindow::updateDoryDatasetList() {
	statusBar()->showMessage(tr("Updating Dory dataset list..."), STATUS_BAR_MSG_TIMEOUT);

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

		this->statusBar()->showMessage(tr("Dory dataset list updated."), STATUS_BAR_MSG_TIMEOUT);
	};

	m_ui->pushButtonUpdateDoryList->setEnabled(false);
	m_ui->pushButtonUpdateDoryList->setText(tr("Updating..."));

	MakeAPIRequest(m_networkManager, "http://navigator.oceansdata.ca/api/datasets/", replyHandler);
}

/***********************************************************************************/
void MainWindow::updateActiveDatasetListWidget() {

	m_ui->listWidgetActiveDatasets->clear();

	for (const auto& datasetName : m_documentRootObject.keys()) {
		m_ui->listWidgetActiveDatasets->addItem(datasetName);
	}

	m_ui->labelActiveConfigFile->setVisible(true);
	m_ui->labelActiveConfigFile->setText(QString("Active Config File: ") + QFileInfo(m_activeConfigFile).fileName());
	m_ui->buttonAddDataset->setEnabled(true);
	m_ui->pushButtonDeleteDataset->setEnabled(true);
}

/***********************************************************************************/
void MainWindow::on_actionPreferences_triggered() {
	DialogPreferences prefsDialog{this};

	prefsDialog.SetPreferences(m_prefs);

	if (prefsDialog.exec()) {
		const auto prevIsOnline = m_prefs.IsOnline;
		m_prefs = prefsDialog.GetPreferences();

		setActiveConfigFile();

		updateActiveDatasetListWidget();

		// Show a notification to restart gUnicorn
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
	QMessageBox::information(this, tr("About"), tr(""));
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
	updateDoryDatasetList();
}

/***********************************************************************************/
void MainWindow::on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item) {
	const auto datasetID{m_datasetsAPIResultCache[item->text()]};
	DialogDatasetView dialog{this};
	bool isUpdatingDownload{false};

	const auto cachedDownloadSettings = m_downloadQueue.find(item->text());
	if (cachedDownloadSettings == m_downloadQueue.end()) {
		dialog.SetData(datasetID, m_networkManager);
	}
	else {
		// The user wants to edit their download settings
		//dialog.SetData();
		isUpdatingDownload = true;
	}

	if (dialog.exec()) {
		const auto data = dialog.GetDownloadData();
		// Only add to queue if variables were selected.
		if (!data.SelectedVariables.empty()) {
			/*
			// Don't accept a giant date range
			std::size_t dayLimit = 60;
			if (m_datasetsAPIResultCache[data.ID]["quantum"] == "month") {
				dayLimit = 1825; // 5 years of monthly data only
			}

			if (data.StartDate.daysTo(data.EndDate) > dayLimit) {
				QMessageBox box{this};
				box.setWindowTitle(tr("Selected date range was too large..."));
				box.setText(tr("For datasets with quantum \"day\" and \"hour\", 60 days is the limit. \nFor datasets with quantum \"month\", 5 years is the limit."));
				box.setIcon(QMessageBox::Critical);
				box.setStandardButtons(QMessageBox::StandardButton::Ok);

				box.exec();

				return;				
			}
			*/
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
	const auto items = m_ui->listWidgetDownloadQueue->selectedItems();

	if (items.empty()) {
		QMessageBox::information(this, tr("Download queue empty"), tr("Your download queue is empty! Add some stuff to download!"));
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

		// Show download stuff
		m_ui->labelDownloadProgress->setVisible(true);
		m_ui->progressBarDownload->setVisible(true);
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
}

/***********************************************************************************/
void MainWindow::on_pushButtonStartWebServer_clicked() {
	if (!m_gunicornRunning) {
		QProcess process{this};
		process.setProgram("/bin/sh");
		process.setWorkingDirectory(m_prefs.ONInstallDir);
		process.setArguments({"runserver.sh", QFileInfo("datasetconfig.json").fileName()});

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

		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(COLOR_GREEN);
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonStopApache_clicked() {
	if (m_apacheRunning) {
		if (!QProcess::startDetached("pkill", {"java"})) {
			QMessageBox box{this};
			box.setWindowTitle(tr("Error"));
			box.setText(tr("Failed to kill gUnicorn process. It might already be stopped. Use ps -aux | grep tomcat to verify."));
			box.setIcon(QMessageBox::Critical);

			box.exec();

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
	m_apacheRunning = IsProcessRunning("tomcat");
	if (!m_apacheRunning && m_prefs.AutoStartServers) {
		//on_pushButtonStartApache_clicked();
		//m_apacheRunning = true;
	}
	if (m_apacheRunning) {
		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(COLOR_GREEN);
	}
}

/***********************************************************************************/
void MainWindow::setActiveConfigFile() {
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
	auto doc = LoadJSONFile(newConfigFile);
	if (doc.isNull()) {
#ifdef QT_DEBUG
		qDebug() << "Config file not found: " << newConfigFile;
		qDebug() << "Creating it...";
#endif
		// File doesn't exist so create it
		WriteJSONFile(newConfigFile);

		// And now load it
		doc = LoadJSONFile(newConfigFile);
	}

	m_activeConfigFile = newConfigFile;
	m_documentRootObject = doc.object();

	// Update UI text
	if (m_prefs.IsOnline) {
		m_ui->labelDatasetTarget->setText(tr("Remote Storage (Dory)"));
	}
	else {
		m_ui->labelDatasetTarget->setText(tr("Local Storage"));
	}

	statusBar()->showMessage(tr("Config file loaded."), STATUS_BAR_MSG_TIMEOUT);
}

/***********************************************************************************/
void MainWindow::on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem* item) {
	delete m_ui->listWidgetDownloadQueue->takeItem(m_ui->listWidgetDownloadQueue->row(item));
}

/***********************************************************************************/
void MainWindow::on_pushButtonUpdateAggregate_clicked() {
	m_ui->pushButtonUpdateAggregate->setEnabled(false);
}
