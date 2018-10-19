#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"

#include "ioutils.h"
#include "preferences.h"

#include <QFileDialog>
#include <QMessageBox>

/***********************************************************************************/
DialogPreferences::DialogPreferences(Preferences& prefs, QWidget* parent) : QDialog{parent},
																			m_ui{new Ui::DialogPreferences},
																			m_prefs{prefs} {
	m_ui->setupUi(this);

	populateUI();
}

/***********************************************************************************/
DialogPreferences::~DialogPreferences() {
	delete m_ui;
}

/***********************************************************************************/
void DialogPreferences::on_pushButtonBrowseInstallDir_clicked() {
#ifdef QT_DEBUG
	const constexpr auto path = "/home/nabil/";
#else
	const constexpr auto path = "/opt/tools/";
#endif
	const auto& dir{ QFileDialog::getExistingDirectory(this,
													   tr("Open Ocean Navigator Installation Folder..."),
													   path,
													   QFileDialog::ShowDirsOnly)
				   };

	if (!dir.isEmpty()) {
		m_ui->lineEditInstallDir->setText(dir);
	}
}

/***********************************************************************************/
void DialogPreferences::on_pushButtonBrowseTHREDDS_clicked() {
	const auto& dir{ QFileDialog::getExistingDirectory(this, tr("Open THREDDS catalog.xml folder...")) };

	if (!dir.isEmpty()) {
		if (IO::FileExists(dir+"/catalog.xml")) {
			m_ui->lineEditTHREDDSDataLocation->setText(dir);
			return;
		}

		QMessageBox::critical(this,
							  tr("Error..."),
							  tr("catalog.xml not found in this directory: ") +
							  dir
							  );
	}
}

/***********************************************************************************/
void DialogPreferences::on_buttonBox_accepted() {
	if (!m_ui->lineEditInstallDir->text().isEmpty()) {
		m_prefs.ONInstallDir = m_ui->lineEditInstallDir->text();
	}

	if (!m_ui->lineEditRemoteURL->text().isEmpty()) {
		m_prefs.RemoteURL = m_ui->lineEditRemoteURL->text();
	}

	if (!m_ui->lineEditTHREDDSDataLocation->text().isEmpty()) {
		m_prefs.THREDDSCatalogLocation = m_ui->lineEditTHREDDSDataLocation->text();
	}

	m_prefs.DataDownloadFormat = m_ui->comboBoxDownloadFormat->currentText();
	m_prefs.UpdateRemoteListOnStart = m_ui->switchUpdateDoryDatasetsOnStart->isChecked();
	m_prefs.IsNetworkOnline = m_ui->switchOnlineOffline->isChecked();
	m_prefs.CheckForUpdatesOnStart = m_ui->switchWidgetCheckForUpdates->isChecked();
}

/***********************************************************************************/
void DialogPreferences::populateUI() {
	const QStringList downloadFormats {
		"NETCDF4", "NETCDF4_CLASSIC", "NETCDF3_64BIT",
		"NETCDF3_CLASSIC", "NETCDF3_NC"
	};

	m_ui->comboBoxDownloadFormat->addItems(downloadFormats);

	m_ui->lineEditInstallDir->setText(m_prefs.ONInstallDir);
	m_ui->lineEditRemoteURL->setText(m_prefs.RemoteURL);
	m_ui->lineEditTHREDDSDataLocation->setText(m_prefs.THREDDSCatalogLocation);
	m_ui->comboBoxDownloadFormat->setCurrentText(m_prefs.DataDownloadFormat);
	m_ui->switchUpdateDoryDatasetsOnStart->setChecked(m_prefs.UpdateRemoteListOnStart);
	m_ui->switchOnlineOffline->setChecked(m_prefs.IsNetworkOnline);
	m_ui->switchWidgetCheckForUpdates->setChecked(m_prefs.CheckForUpdatesOnStart);
}
