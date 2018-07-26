#include "dialogthreddslogs.h"
#include "ui_dialogthreddslogs.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDesktopServices>

/***********************************************************************************/
DialogTHREDDSLogs::DialogTHREDDSLogs(const QString& threddsLogFolder, QWidget* parent) :	QDialog{parent},
																							m_ui{new Ui::DialogTHREDDSLogs},
																							m_logFolder{threddsLogFolder} {
	m_ui->setupUi(this);

	const QDir dir{m_logFolder};
	const auto& logList{ dir.entryInfoList({"*.log"}) };

	for (const auto& log : logList) {
		m_ui->comboBoxLogFiles->addItem(log.fileName());
	}

	m_reloadLogFileTimer.setInterval(30000); // Reload log file every 30 secs.
	QObject::connect(&m_reloadLogFileTimer, &QTimer::timeout, this, [this](){ this->loadLogFile(this->m_ui->comboBoxLogFiles->currentText()); });
	m_reloadLogFileTimer.start();
}

/***********************************************************************************/
DialogTHREDDSLogs::~DialogTHREDDSLogs() {
	delete m_ui;
}

/***********************************************************************************/
void DialogTHREDDSLogs::on_pushButtonOpenLogFolder_clicked() {
	const auto& path{ QDir::toNativeSeparators(m_logFolder) };
	QDesktopServices::openUrl(path);
}

/***********************************************************************************/
void DialogTHREDDSLogs::on_comboBoxLogFiles_currentIndexChanged(const QString& arg1) {
	loadLogFile(arg1);
}

/***********************************************************************************/
void DialogTHREDDSLogs::loadLogFile(const QString& filename) {
	m_ui->textBrowser->clear();

	QFile f{ m_logFolder + "/" + filename };
	f.open(QFile::ReadOnly | QFile::Text);

	QTextStream ts{ &f };

	m_ui->textBrowser->append(ts.readAll());
}

/***********************************************************************************/
void DialogTHREDDSLogs::on_pushButtonReloadLogFile_clicked() {
	m_ui->comboBoxLogFiles->setEnabled(false);
	loadLogFile(m_ui->comboBoxLogFiles->currentText());
	m_ui->comboBoxLogFiles->setEnabled(true);
}
