#ifndef DIALOGTHREDDSLOGS_H
#define DIALOGTHREDDSLOGS_H

#include <QDialog>
#include <QTimer>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class DialogTHREDDSLogs;
}

/***********************************************************************************/
class DialogTHREDDSLogs : public QDialog {
	Q_OBJECT

public:
	DialogTHREDDSLogs(const QString& threddsLogFolder, QWidget* parent = nullptr);
	~DialogTHREDDSLogs();

private slots:
	void on_pushButtonOpenLogFolder_clicked();

	void on_comboBoxLogFiles_currentIndexChanged(const QString& arg1);

private:
	//
	void loadLogFile(const QString& filename);

	Ui::DialogTHREDDSLogs* m_ui{nullptr};

	QTimer m_reloadLogFileTimer{this};

	const QString m_logFolder;
};

#endif // DIALOGTHREDDSLOGS_H
