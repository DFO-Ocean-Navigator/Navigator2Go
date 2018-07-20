#ifndef DIALOGTHREDDSLOGS_H
#define DIALOGTHREDDSLOGS_H

#include <QDialog>

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
	Ui::DialogTHREDDSLogs* m_ui{nullptr};

	const QString m_logFolder;
};

#endif // DIALOGTHREDDSLOGS_H
