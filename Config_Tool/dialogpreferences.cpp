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
}

/***********************************************************************************/
auto DialogPreferences::GetPreferences() const noexcept -> Preferences {
	Preferences prefs;
	prefs.ONInstallDir = m_ui->lineEditInstallDir->text();
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
