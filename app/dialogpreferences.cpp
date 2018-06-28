#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"

#include <QFileDialog>

/***********************************************************************************/
DialogPreferences::DialogPreferences(QWidget* parent) : QDialog{parent}, m_ui{new Ui::DialogPreferences} {
	m_ui->setupUi(this);
}

/***********************************************************************************/
DialogPreferences::~DialogPreferences() {
	delete m_ui;
}

/***********************************************************************************/
void DialogPreferences::SetPreferences(Preferences& settings) {
	m_ui->lineEditInstallDir->setText(settings.ONInstallDir);
	m_ui->lineEditRemoteURL->setText(settings.RemoteURL);
	m_ui->lineEditTHREDDSDataLocation->setText(settings.THREDDSDataLocation);
	m_ui->switchUpdateDoryDatasetsOnStart->setChecked(settings.UpdateRemoteListOnStart);
	m_ui->switchAutoStartServers->setChecked(settings.AutoStartServers);
	m_ui->switchOnlineOffline->setChecked(settings.IsOnline);
}

/***********************************************************************************/
auto DialogPreferences::GetPreferences() const noexcept -> Preferences {
	return {
		m_ui->lineEditInstallDir->text(),
		m_ui->lineEditRemoteURL->text(),
		m_ui->lineEditTHREDDSDataLocation->text(),
		m_ui->switchUpdateDoryDatasetsOnStart->isChecked(),
		m_ui->switchAutoStartServers->isChecked(),
		m_ui->switchOnlineOffline->isChecked()
	};
}

/***********************************************************************************/
void DialogPreferences::on_pushButtonBrowseInstallDir_clicked() {
#ifdef QT_DEBUG
	const auto path = "/home/nabil/";
#else
	const auto path = "/opt/tools/";
#endif
	const auto dir = QFileDialog::getExistingDirectory(this,
													   tr("Open Ocean Navigator Installation Folder..."),
													   path,
													   QFileDialog::ShowDirsOnly);
	if (!dir.isEmpty()) {
		m_ui->lineEditInstallDir->setText(dir);
	}
}

/***********************************************************************************/
void DialogPreferences::on_pushButtonBroweseTHREDDS_clicked() {
	const auto dir = QFileDialog::getExistingDirectory(this, tr("Open THREDDS Dataset Folder..."));

	if (!dir.isEmpty()) {
		m_ui->lineEditTHREDDSDataLocation->setText(dir);
	}
}
