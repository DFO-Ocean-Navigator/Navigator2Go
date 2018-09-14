#include "widgetdashboard.h"
#include "ui_widgetdashboard.h"

#include "mainwindow.h"
#include "dialogimportnc.h"

#include "constants.h"
#include "ioutils.h"
#include "systemutils.h"
#include "preferences.h"
#include "network.h"
#include "xmlio.h"

#include <QMessageBox>
#include <QStorageInfo>
#include <QFileInfo>
#include <QFileDialog>
#include <QThreadPool>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
WidgetDashboard::WidgetDashboard(QWidget* parent, MainWindow* mainWindow, const Preferences* prefs) :	QWidget{parent},
																										m_ui{new Ui::WidgetDashboard},
																										m_mainWindow{mainWindow},
																										m_prefs{prefs} {
	m_ui->setupUi(this);

	checkAndStartServers();

	m_ui->labelDatasetTarget->setStyleSheet(QSS_COLOR_GREEN);

	updateDriveInfo();

	m_consoleProcess.setProgram("/bin/sh");
}

/***********************************************************************************/
WidgetDashboard::~WidgetDashboard() {
	delete m_ui;
}

/***********************************************************************************/
void WidgetDashboard::checkAndStartServers() {
	// gUnicorn
	m_isGunicornRunning = System::IsProcessRunning("gunicorn");
	if (!m_isGunicornRunning && m_prefs->AutoStartServers) {
		on_pushButtonStartWebServer_clicked();
		m_isGunicornRunning = true;
	}
	if (m_isGunicornRunning) {
		m_ui->labelStatusWebServer->setText(tr("Running"));
		m_ui->labelStatusWebServer->setStyleSheet(QSS_COLOR_GREEN);
	}

	// Apache tomcat
	m_apacheRunning = System::IsProcessRunning("java");
	if (!m_apacheRunning && m_prefs->AutoStartServers) {
		on_pushButtonStartApache_clicked();
		m_apacheRunning = true;
	}
	if (m_apacheRunning) {
		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(QSS_COLOR_GREEN);
	}
}

/***********************************************************************************/
void WidgetDashboard::showOnlineText() {
	m_ui->labelRemoteUplink->setText(tr("Online"));
	m_ui->labelRemoteUplink->setStyleSheet(QSS_COLOR_GREEN);

	m_ui->labelDatasetTarget->setText(tr("Remote Storage: \n") + m_prefs->RemoteURL);
}

/***********************************************************************************/
void WidgetDashboard::showOfflineText() {
	m_ui->labelRemoteUplink->setText(tr("Offline"));
	m_ui->labelRemoteUplink->setStyleSheet(QSS_COLOR_RED);

	m_ui->labelDatasetTarget->setText(tr("Local Storage"));
}

/***********************************************************************************/
void WidgetDashboard::enableUplinkTestButton() {
	m_ui->pushButtonCheckRemoteUplink->setEnabled(true);
	m_ui->pushButtonCheckRemoteUplink->setText(tr("Test"));
}

/***********************************************************************************/
void WidgetDashboard::disableUplinkTestButton() {
	m_ui->pushButtonCheckRemoteUplink->setEnabled(false);
	m_ui->pushButtonCheckRemoteUplink->setText(tr("Checking..."));
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonStartWebServer_clicked() {
	const auto& datasetConfigFile{ m_prefs->IsNetworkOnline ? "datasetconfigONLINE.json" : "datasetconfigOFFLINE.json" };

	if (!m_isGunicornRunning) {
		m_consoleProcess.setWorkingDirectory(m_prefs->ONInstallDir);
		m_consoleProcess.setArguments({"runserver.sh", datasetConfigFile});

		if (!m_consoleProcess.startDetached(nullptr)) {
			QMessageBox box{this};
			box.setWindowTitle(tr("Error"));
			box.setText(tr("Failed to start gUnicorn."));
			box.setDetailedText(tr("Check the permissions of the /tmp/oceannavigator folder."));
			box.setIcon(QMessageBox::Critical);

			box.exec();

			return;
		}

		m_ui->labelStatusWebServer->setText(tr("Running"));
		m_ui->labelStatusWebServer->setStyleSheet(QSS_COLOR_GREEN);
		m_isGunicornRunning = true;
	}
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonStopWebServer_clicked() {
	if (m_isGunicornRunning) {
		if (!QProcess::startDetached("pkill", {"gunicorn"})) {
			QMessageBox box{this};
			box.setWindowTitle(tr("Error"));
			box.setText(tr("Failed to kill gUnicorn process. It might already be stopped. Use ps -aux | grep gunicorn to verify."));
			box.setIcon(QMessageBox::Critical);

			box.exec();

			return;
		}
		m_ui->labelStatusWebServer->setText(tr("Stopped"));
		m_ui->labelStatusWebServer->setStyleSheet(QSS_COLOR_RED);
		m_isGunicornRunning = false;
	}
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonStopApache_clicked() {
	if (m_apacheRunning) {
		m_consoleProcess.setWorkingDirectory(IO::TOMCAT_BIN_DIR);
		m_consoleProcess.setArguments({"shutdown.sh"});

		if (!m_consoleProcess.startDetached()) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to stop THREDDS server."));

			return;
		}

		m_ui->labelStatusApache->setText(tr("Stopped"));
		m_ui->labelStatusApache->setStyleSheet(QSS_COLOR_RED);
		m_apacheRunning = false;
	}
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonStartApache_clicked() {
	if (!m_apacheRunning) {
		m_consoleProcess.setWorkingDirectory(IO::TOMCAT_BIN_DIR);
		m_consoleProcess.setArguments({"startup.sh"});

		if (!m_consoleProcess.startDetached()) {
			QMessageBox::critical(this, tr("Error"), tr("Failed to start THREDDS server."));

			return;
		}

		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(QSS_COLOR_GREEN);
		m_apacheRunning = true;
	}
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonImportNetCDF_clicked() {
	DialogImportNC dlg{m_prefs->THREDDSCatalogLocation, this};

	if (dlg.exec()) {

		auto&& importList{ dlg.GetImportList() };
		if (importList.empty()) {
			return;
		}

		auto* const task{ new IO::CopyFilesRunnable(m_prefs->THREDDSCatalogLocation, dlg.ShouldRemoveSourceNCFiles(), std::move(importList)) };

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

			m_ui->pushButtonImportNetCDF->setEnabled(true);
			m_mainWindow->hideProgressBar();
		});

		// Update progress bar state
		QObject::connect(task, &IO::CopyFilesRunnable::progress, this, [&](const auto progress) {
			m_mainWindow->updateProgressBar(static_cast<int>(progress));
		});

		m_ui->pushButtonImportNetCDF->setEnabled(false);

		m_mainWindow->showProgressBar("NetCDF Import Progress:");

		// Run it
		QThreadPool::globalInstance()->start(task);
	}
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonCheckRemoteUplink_clicked() {
	m_mainWindow->checkRemoteConnection();
}

/***********************************************************************************/
void WidgetDashboard::updateDriveInfo() {

	const auto& storage{ QStorageInfo::root() };
	const auto total{ storage.bytesTotal()/1000/1000/1000 };
	const auto available{ storage.bytesAvailable()/1000/1000/1000 };
	const auto used{ total - available };

	m_ui->labelHDDName->setText(storage.name() + QString(" Usage: %1GB | Available: %2GB | Total: %3GB").arg(used).arg(available).arg(total));
	m_ui->progressBarDriveSpace->setMinimum(0);
	m_ui->progressBarDriveSpace->setMaximum(static_cast<int>(total));
	m_ui->progressBarDriveSpace->setValue(static_cast<int>(used));
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonClearPythonCache_clicked() {
	if (QMessageBox::warning(this,
							 tr("Confirm"),
							 tr("Are you sure you wish to clear the python cache? This will result in a temporary slow-down of the Navigator web UI."),
							 QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {


		QDir d{"/tmp/oceannavigator"};
		if (!d.exists()) {
			m_mainWindow->showStatusBarMessage("Python cache empty...nothing to do.");
			return;
		}

		QString status;
		d.removeRecursively() ? status = "successful" : status = "failed";

		QMessageBox::information(this,
								 tr("Clear python cache"),
								 tr("Cache clearing operation ") + status + ".");
	}
}
