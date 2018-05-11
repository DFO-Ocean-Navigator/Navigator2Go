#include "dialogpreferences.h"
#include "ui_dialogpreferences.h"

#include <QFileDialog>

/***********************************************************************************/
DialogPreferences::DialogPreferences(QWidget *parent) : QDialog(parent), m_ui(new Ui::DialogPreferences) {
	m_ui->setupUi(this);
}

/***********************************************************************************/
DialogPreferences::~DialogPreferences() {
	delete m_ui;
}

/***********************************************************************************/
void DialogPreferences::SetPreferences(Preferences& settings) {
	m_ui->lineEditInstallDir->setText(settings.ONInstallDir);
	m_ui->checkBoxUpdateDoryListOnStart->setCheckState(settings.UpdateDoryListOnStart ? Qt::Checked : Qt::Unchecked);
	if (settings.ONActiveDatasetConfig.isEmpty()) {
		m_ui->lineEditActiveConfigFile->setText(settings.ONInstallDir+"/oceannavigator/datasetconfig.json");
	} else {
		m_ui->lineEditActiveConfigFile->setText(settings.ONActiveDatasetConfig);
	}
}

/***********************************************************************************/
auto DialogPreferences::GetPreferences() const noexcept -> Preferences {
	Preferences prefs;
	prefs.ONInstallDir = m_ui->lineEditInstallDir->text();
	prefs.ONActiveDatasetConfig = m_ui->lineEditActiveConfigFile->text();
	const auto checked = m_ui->checkBoxUpdateDoryListOnStart->checkState();
	prefs.UpdateDoryListOnStart = checked == Qt::Checked ? true : false;

	return prefs;
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
													   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		m_ui->lineEditInstallDir->setText(dir);
	}
}

/***********************************************************************************/
void DialogPreferences::on_pushButtonBrowseActiveConfigFile_clicked() {
	QFileDialog dialog{this};
	dialog.setWindowTitle(tr("Open Dataset Config File"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Config Files (*.json)"));
	dialog.setViewMode(QFileDialog::Detail);
#ifdef QT_DEBUG
	dialog.setDirectory("/home/nabil/");
#else
	dialog.setDirectory("/opt/tools/");
#endif

	// Open file dialog
	if (dialog.exec()) {
		m_ui->lineEditActiveConfigFile->setText(dialog.selectedFiles().at(0));
	}
}
