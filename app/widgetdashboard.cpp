#include "widgetdashboard.h"
#include "ui_widgetdashboard.h"

#include "mainwindow.h"

#include "defines.h"
#include "ioutils.h"
#include "process.h"
#include "preferences.h"
#include "network.h"

#include <QProcess>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QThreadPool>

#include <QDebug>

/***********************************************************************************/
WidgetDashboard::WidgetDashboard(QWidget* parent, MainWindow* mainWindow, const Preferences* prefs) :	QWidget{parent},
																										m_ui{new Ui::WidgetDashboard},
																										m_mainWindow{mainWindow},
																										m_prefs{prefs} {
	m_ui->setupUi(this);

	checkAndStartServers();

	m_ui->labelDatasetTarget->setStyleSheet(COLOR_GREEN);
	m_ui->labelUpdate->setStyleSheet(COLOR_GREEN);
	m_ui->labelUpdate->setVisible(false);
	m_ui->pushButtonUpdate->setEnabled(false);
	m_ui->pushButtonUpdate->setVisible(false);
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
		m_ui->labelStatusWebServer->setStyleSheet(COLOR_GREEN);
	}

	// Apache tomcat
	m_apacheRunning = System::IsProcessRunning("java");
	if (!m_apacheRunning && m_prefs->AutoStartServers) {
		on_pushButtonStartApache_clicked();
		m_apacheRunning = true;
	}
	if (m_apacheRunning) {
		m_ui->labelStatusApache->setText(tr("Running"));
		m_ui->labelStatusApache->setStyleSheet(COLOR_GREEN);
	}
}

/***********************************************************************************/
void WidgetDashboard::showOnlineText() {
	m_ui->labelRemoteUplink->setText(tr("Online"));
	m_ui->labelRemoteUplink->setStyleSheet(COLOR_GREEN);

	m_ui->labelDatasetTarget->setText(tr("Remote Storage: \n") + m_prefs->RemoteURL);
}

/***********************************************************************************/
void WidgetDashboard::showOfflineText() {
	m_ui->labelRemoteUplink->setText(tr("Offline"));
	m_ui->labelRemoteUplink->setStyleSheet(COLOR_RED);

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
	if (!m_isGunicornRunning) {
		QProcess process{this};
		process.setProgram("/bin/sh");
		process.setWorkingDirectory(m_prefs->ONInstallDir);
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
		m_ui->labelStatusWebServer->setStyleSheet(COLOR_RED);
		m_isGunicornRunning = false;
	}
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonStopApache_clicked() {
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
void WidgetDashboard::on_pushButtonStartApache_clicked() {
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
void WidgetDashboard::on_pushButtonImportNetCDF_clicked() {
	auto files = QFileDialog::getOpenFileNames(this,
													 tr("Select NetCDF files to import..."),
													 QDir::currentPath(),
													 "NetCDF Files (*.nc)"
													 );
	qDebug() << files;
	if (files.empty()) {
		return;
	}

	auto* task{ new IO::CopyFilesRunnable(std::move(files)) };

	QObject::connect(task, &IO::CopyFilesRunnable::finished, this, [&](const auto errorList) {
		if (!errorList.empty()) {
			qDebug() << "ERRORS OCCOURED";
		}
		else {

		}

		m_ui->pushButtonImportNetCDF->setEnabled(true);
	});

	QObject::connect(task, &IO::CopyFilesRunnable::progress, this, [&](const auto progress) {
		qDebug() << progress;
	});

	m_ui->pushButtonImportNetCDF->setEnabled(false);

	// Run it
	QThreadPool::globalInstance()->start(task);
}

/***********************************************************************************/
void WidgetDashboard::on_pushButtonCheckRemoteUplink_clicked() {
	m_mainWindow->checkRemoteConnection();
}