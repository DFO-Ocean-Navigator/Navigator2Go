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
	m_ui->textBrowser->clear();

	QFile f{ m_logFolder + "/" + arg1 };
	f.open(QFile::ReadOnly | QFile::Text);

	QTextStream ts{ &f };

	m_ui->textBrowser->append(ts.readAll());
}
