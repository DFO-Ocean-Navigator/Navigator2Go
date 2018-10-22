#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogdatasetview.h"
#include "dialogpreferences.h"
#include "dialogimportnc.h"
#include "widgetconfigeditor.h"
#include "widgetdataorder.h"
#include "widgetthreddsconfig.h"
#include "updaterunnable.h"

#include "network.h"
#include "jsonio.h"
#include "systemutils.h"
#include "ioutils.h"
#include "xmlio.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QJsonArray>
#include <QNetworkReply>
#include <QThreadPool>
#include <QDesktopServices>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
auto appVersion() noexcept { return APP_VERSION; }
auto commitHash() noexcept { return GIT_CURRENT_SHA1; }
auto currentBranch() noexcept { return GIT_CURRENT_BRANCH; }

/***********************************************************************************/
MainWindow::MainWindow(QWidget* parent) : 	QMainWindow{parent},
											m_ui{new Ui::MainWindow} {
	m_ui->setupUi(this);
	// Set dark stylesheet
	QFile f{QStringLiteral(":qdarkstyle/style.qss")};
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

	if (m_prefs.FirstRun) {
#ifdef QT_DEBUG
		qDebug() << "First run";
#endif
		showFirstRunConfiguration();

		QMessageBox::information(this,
								 tr("Add some datasets!"),
								 tr("Click on the Data Order button to get some data."));
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

	if (m_prefs.CheckForUpdatesOnStart) {
		checkForUpdates();
	}

	setWindowTitle(tr("Navigator2Go"));

	if (!m_prefs.AdvancedUI) {
		m_ui->tabWidget->removeTab(3);
		m_ui->tabWidget->removeTab(2);
	}

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
void MainWindow::initWidgets() {

	m_widgetConfigEditor = new WidgetConfigEditor(m_ui->tabWidget, this, &m_prefs);

	m_widgetDataOrder = new WidgetDataOrder(m_ui->tabWidget, this, m_prefs);
	QObject::connect(m_widgetDataOrder, &WidgetDataOrder::refreshRequested, this, &MainWindow::refreshRequestHandler);

	m_widgetThreddsConfig = new WidgetThreddsConfig(m_ui->tabWidget, &m_prefs);
}

/***********************************************************************************/
void MainWindow::on_actionPreferences_triggered() {
	DialogPreferences prefsDialog{m_prefs, this};

	if (prefsDialog.exec()) {
		// Store previous network state
		const auto prevNetworkState{ m_prefs.IsNetworkOnline };

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
							 QString("Navigator2Go Version: %1\nGit commit hash: %2\nCompiled from branch: %3").arg(appVersion())
																											   .arg(commitHash())
																											   .arg(currentBranch())
							 );
}

/***********************************************************************************/
void MainWindow::setInitialLayout() {
	m_ui->tabWidget->setCurrentIndex(0);

	hideProgressBar();

	// Add widgets to their tabs
	m_ui->dataOrderLayout->addWidget(m_widgetDataOrder);
	m_ui->configEditorLayout->addWidget(m_widgetConfigEditor);
	m_ui->threddsConfigLayout->addWidget(m_widgetThreddsConfig);
}

/***********************************************************************************/
void MainWindow::checkForUpdates() {
	checkForAppUpdate();
	checkForONUpdates();
}

/***********************************************************************************/
void MainWindow::checkForAppUpdate() {
	showStatusBarMessage("Checking for Navigator2Go updates...");
	auto* const nam{ new QNetworkAccessManager() };
	QNetworkRequest req{{QStringLiteral("https://raw.githubusercontent.com/DFO-Ocean-Navigator/Navigator2Go/master/VERSION.txt")}};

	m_updateReply = nam->get(req);

	QObject::connect(m_updateReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, [&](const auto errorCode) {
		m_updateReply->deleteLater();
	});
	QObject::connect(m_updateReply, &QNetworkReply::finished, this, [&]() {
		const QString& b{ m_updateReply->readAll() };
		const auto number{ b.split("=", QString::SplitBehavior::SkipEmptyParts)[1].simplified() };
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Information);

		if (number == APP_VERSION) {
			msgBox.setWindowTitle(tr("Navigator2Go"));
			msgBox.setText(tr("No updates available!"));
		} else {
			msgBox.setWindowTitle(tr("Navigator2Go Update Available!"));
			msgBox.setTextFormat(Qt::RichText);
			msgBox.setText(QStringLiteral("<a href='https://github.com/DFO-Ocean-Navigator/Navigator2Go/releases/latest' style='color: #3daee9'>https://github.com/DFO-Ocean-Navigator/Navigator2Go/releases/latest</a>"));
		}

		msgBox.exec();

		m_updateReply->deleteLater();
	});
}

/***********************************************************************************/
void MainWindow::checkForONUpdates() {
	showStatusBarMessage("Checking for Ocean Navigator updates...");
	auto* const task{ new UpdateRunnable{m_prefs.ONInstallDir} };

	QObject::connect(task, &UpdateRunnable::finished, this, [&](){
		QMessageBox::information(this,
								 tr("Update check complete..."),
								 tr("Restart the gUnicorn server from the dashboard, and refresh your browser page to apply the changes."));
	});

	QThreadPool::globalInstance()->start(task);
}

/***********************************************************************************/
void MainWindow::setOnline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to online.";
#endif
	m_widgetDataOrder->setNAMOnline();
	m_widgetConfigEditor->updateDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::setOffline() {
#ifdef QT_DEBUG
	qDebug() << "Changing to offline.";
#endif
	m_widgetDataOrder->setNAMOffline();
	m_widgetConfigEditor->updateDatasetListWidget();
}

/***********************************************************************************/
void MainWindow::checkRemoteConnection() {
	if (m_prefs.IsNetworkOnline) {
		// QThreadPool deletes automatically
		auto* const task{ new Network::URLExistsRunnable{m_prefs.RemoteURL, 80} };

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

		}, Qt::BlockingQueuedConnection); // <-- Check out this magic...this would segfault otherwise

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
void MainWindow::updateTHREDDSConfigTable() {
	m_widgetThreddsConfig->BuildTable();
}

/***********************************************************************************/
void MainWindow::refreshRequestHandler() {
	updateTHREDDSConfigTable();

	on_pushButtonRefresh_clicked();
}

/***********************************************************************************/
void MainWindow::on_actionCheck_for_Updates_triggered() {
	checkForUpdates();
}

/***********************************************************************************/
void MainWindow::showFirstRunConfiguration() {
	QMessageBox::information(this,
							 tr("Navigator2Go First Run"),
							 tr("Hello there! It appears this is the first time you are running Navigator2Go on this computer. Go to: <a href='https://dfo-ocean-navigator.github.io/Navigator2GoTutorial/' style='color: #3daee9'>https://dfo-ocean-navigator.github.io/Navigator2GoTutorial/</a>"));

	DialogPreferences prefsDialog{m_prefs, this};
	prefsDialog.setWindowTitle(tr("Navigator2Go Initial Setup..."));

	do {
		prefsDialog.exec();
	}
	while(m_prefs.THREDDSCatalogLocation.isEmpty());

}

/***********************************************************************************/
void MainWindow::on_actionNavigator2Go_Manual_triggered() {
	QDesktopServices::openUrl({QStringLiteral("https://dfo-ocean-navigator.github.io/Navigator2GoTutorial/")});
}

/***********************************************************************************/
void MainWindow::on_pushButtonLaunchViewer_clicked() {
	QDesktopServices::openUrl({QStringLiteral("http://localhost:5000")});
}

/***********************************************************************************/
void MainWindow::on_pushButtonOpenDataOrder_clicked() {
	m_ui->tabWidget->setCurrentIndex(1);
}

/***********************************************************************************/
void MainWindow::on_pushButtonImportNCFiles_clicked() {
	DialogImportNC dlg{m_prefs.THREDDSCatalogLocation, this};

	if (dlg.exec()) {

		auto&& importList{ dlg.GetImportList() };
		if (importList.empty()) {
			return;
		}

		auto* const task{ new IO::CopyFilesRunnable(m_prefs.THREDDSCatalogLocation, dlg.ShouldRemoveSourceNCFiles(), std::move(importList)) };

		QObject::connect(task, &IO::CopyFilesRunnable::finished, this, [&](const auto errorList) {
			QMessageBox box{this};
			if (!errorList.empty()) {
				box.setWindowTitle(tr("Errors occoured..."));
				box.setIcon(QMessageBox::Warning);
				box.setText(tr("The following files failed to import: "));
				box.setDetailedText(errorList.join('\n'));
			}
			else {
				box.setWindowTitle(tr("Import successful!"));
				box.setIcon(QMessageBox::Information);
				box.setText(tr("Your files have successfully been imported into the THREDDS server. Please go to the THREDDS Config tab to verify. Then add your datasets in the Dataset Config Editor"));
			}
			box.exec();

			m_ui->pushButtonImportNCFiles->setEnabled(true);
			hideProgressBar();
		});

		// Update progress bar state
		QObject::connect(task, &IO::CopyFilesRunnable::progress, this, [&](const auto progress) {
			updateProgressBar(static_cast<int>(progress));
		});

		m_ui->pushButtonImportNCFiles->setEnabled(false);

		showProgressBar("NetCDF Import Progress:");

		// Run it
		QThreadPool::globalInstance()->start(task);
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonViewLocalData_clicked() {

}

/***********************************************************************************/
void MainWindow::on_actionClear_Python_cache_triggered() {
	IO::ClearPythonCache();
}

/***********************************************************************************/
void MainWindow::on_pushButtonRefresh_clicked() {
	m_ui->pushButtonRefresh->setEnabled(false);

	IO::ClearPythonCache();

	m_serverManager.refreshServers();

	m_ui->pushButtonRefresh->setEnabled(true);
}
