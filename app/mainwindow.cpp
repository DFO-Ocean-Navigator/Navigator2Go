#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogdatasetview.h"
#include "dialogpreferences.h"
#include "widgetdashboard.h"
#include "widgetconfigeditor.h"
#include "widgetdataorder.h"
#include "widgetthreddsconfig.h"

#include "network.h"
#include "jsonio.h"
#include "process.h"
#include "ioutils.h"
#include "constants.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QJsonArray>
#include <QSettings>
#include <QNetworkReply>
#include <QThreadPool>

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

	initWidgets();

	checkRemoteConnection();

	m_uplinkTimer.setInterval(300000); // Check for remote uplink every 5 minutes.
	QObject::connect(&m_uplinkTimer, &QTimer::timeout, this, &MainWindow::checkRemoteConnection);
	m_uplinkTimer.start();

	if (m_prefs.IsNetworkOnline) {
		setOnline();

		if (m_prefs.UpdateRemoteListOnStart) {
			m_widgetDataOrder->updateRemoteDatasetListWidget();
		}
	} else {
		setOffline();
	}

	checkForUpdates();

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
void MainWindow::initWidgets() {

	m_widgetDashboard = new WidgetDashboard(m_ui->tabWidget, this, &m_prefs);

	m_widgetConfigEditor = new WidgetConfigEditor(m_ui->tabWidget, this, &m_prefs);

	m_widgetDataOrder = new WidgetDataOrder(m_ui->tabWidget, this, &m_prefs);
	setDataOrderRegion();

	m_widgetThreddsConfig = new WidgetThreddsConfig(m_ui->tabWidget, &m_prefs);
}

/***********************************************************************************/
void MainWindow::readSettings() {

	// General
	m_settings.beginGroup("General");

	if (!m_settings.contains("FirstRun")) {
		m_firstRun = true;
	}

	if (m_settings.contains("ONInstallDir")) {
		m_prefs.ONInstallDir = m_settings.value("ONInstallDir").toString();
	}
	else {
		m_prefs.ONInstallDir = "/opt/Ocean-Data-Map-Project/";
	}

	if (m_settings.contains("RemoteURL")) {
		m_prefs.RemoteURL = m_settings.value("RemoteURL").toString();
	}
	else {
		m_prefs.RemoteURL = "http://www.navigator.oceansdata.ca/";
	}

	if (m_settings.contains("THREDDSCatalogLocation")) {
		m_prefs.THREDDSCatalogLocation = m_settings.value("THREDDSCatalogLocation").toString();
	}

	if (m_settings.contains("UpdateRemoteListOnStart")) {
		m_prefs.UpdateRemoteListOnStart = m_settings.value("UpdateRemoteListOnStart").toBool();
	}

	if (m_settings.contains("AutoStartServers")) {
		m_prefs.AutoStartServers = m_settings.value("AutoStartServers").toBool();
	}

	if (m_settings.contains("IsNetworkOnline")) {
		m_prefs.IsNetworkOnline = m_settings.value("IsNetworkOnline").toBool();
	}

	m_settings.endGroup();

	//
	m_settings.beginGroup("DataOrder");

	if (m_settings.contains("DataDownloadFormat")) {
		m_prefs.DataDownloadFormat = m_settings.value("DataDownloadFormat").toString();
	}

	m_settings.endGroup();

	//
	m_settings.beginGroup("Jobs");
	m_settings.endGroup();
}

/***********************************************************************************/
void MainWindow::writeSettings() {
	// General
	m_settings.beginGroup("General");

	m_settings.setValue("FirstRun", false);
	m_settings.setValue("ONInstallDir", m_prefs.ONInstallDir);
	m_settings.setValue("RemoteURL", m_prefs.RemoteURL);
	m_settings.setValue("THREDDSCatalogLocation", m_prefs.THREDDSCatalogLocation);
	m_settings.setValue("UpdateRemoteListOnStart", m_prefs.UpdateRemoteListOnStart);
	m_settings.setValue("AutoStartServers", m_prefs.AutoStartServers);
	m_settings.setValue("IsNetworkOnline", m_prefs.IsNetworkOnline);

	m_settings.endGroup();

	// DataOrder
	m_settings.beginGroup("DataOrder");

	const auto& region = m_widgetDataOrder->getRegion();
	m_settings.setValue("MinLat", std::get<0>(region));
	m_settings.setValue("MaxLat", std::get<1>(region));
	m_settings.setValue("MinLon", std::get<2>(region));
	m_settings.setValue("MaxLon", std::get<3>(region));

	m_settings.setValue("DataDownloadFormat", m_prefs.DataDownloadFormat);

	m_settings.endGroup();

	// Jobs
	m_settings.beginGroup("Jobs");
	m_settings.endGroup();
}

/***********************************************************************************/
void MainWindow::setDataOrderRegion() {
	// DataOrder
	m_settings.beginGroup("DataOrder");

	// If the settings has MinLat, it has all the points
	if (m_settings.contains("MinLat")) {
		m_widgetDataOrder->setRegion(m_settings.value("MinLat").toDouble(),
									 m_settings.value("MaxLat").toDouble(),
									 m_settings.value("MinLon").toDouble(),
									 m_settings.value("MaxLon").toDouble()
									);
	}

	m_settings.endGroup();
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
	QMessageBox::information(this,
							 tr("About Navigator2Go"),
							 QString("Git commit hash: %1\nCompiled from branch: %2").arg(GIT_CURRENT_SHA1).arg(GIT_CURRENT_BRANCH)
							 );
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
void MainWindow::setInitialLayout() {
	m_ui->tabWidget->setCurrentIndex(UITabs::HOME);

	hideProgressBar();

	// Add widgets to their tabs
	m_ui->dashboardLayout->addWidget(m_widgetDashboard);
	m_ui->dataOrderLayout->addWidget(m_widgetDataOrder);
	m_ui->configEditorLayout->addWidget(m_widgetConfigEditor);
	m_ui->threddsConfigLayout->addWidget(m_widgetThreddsConfig);
}

/***********************************************************************************/
void MainWindow::checkForUpdates() {
}

/***********************************************************************************/
void MainWindow::setOnline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to online.";
#endif
	m_widgetDataOrder->setNAMOnline();
	m_widgetDashboard->showOnlineText();
	m_widgetConfigEditor->setDefaultConfigFile();
	m_widgetConfigEditor->updateDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::setOffline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to offline.";
#endif
	m_widgetDataOrder->setNAMOffline();
	m_widgetDashboard->showOfflineText();
	m_widgetConfigEditor->setDefaultConfigFile();
	m_widgetConfigEditor->updateDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::checkRemoteConnection() {
	if (m_prefs.IsNetworkOnline) {
		// QThreadPool deletes automatically
		auto* const task{ new Network::URLExistsRunnable{m_prefs.RemoteURL} };

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
				this->m_widgetDataOrder->updateRemoteDatasetListWidget();
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
void MainWindow::showProgressBar(const char* labelText) {
	m_ui->labelProgressBar->setVisible(true);
	m_ui->labelProgressBar->setText(tr(labelText));

	m_ui->progressBar->setVisible(true);
	m_ui->progressBar->setValue(0);
}

/***********************************************************************************/
void MainWindow::updateProgressBar(const int value) {
	m_ui->progressBar->setValue(value);
}

/***********************************************************************************/
void MainWindow::hideProgressBar() {
	m_ui->labelProgressBar->setVisible(false);
	m_ui->progressBar->setVisible(false);
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

	while(prefsDialog.GetPreferences().THREDDSCatalogLocation.isEmpty()) {
		prefsDialog.exec();
	}

	m_prefs = prefsDialog.GetPreferences();
}
