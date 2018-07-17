#ifndef DIALOGIMPORTNC_H
#define DIALOGIMPORTNC_H

#include "netcdfimportdesc.h"

#include <QDialog>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class DialogImportNC;
}

/***********************************************************************************/
class DialogImportNC : public QDialog {
	Q_OBJECT

public:
	DialogImportNC(const QString& threddsContentDir, QWidget* parent = nullptr);
	~DialogImportNC();

	NODISCARD QVector<NetCDFImportDesc> GetImportList() const;

private slots:
	void on_pushButtonAdd_clicked();
	void on_pushButtonDelete_clicked();
	void on_tableWidget_doubleClicked(const QModelIndex& index);

private:
	//
	void addDataset();

	Ui::DialogImportNC* m_ui{nullptr};
	QStringList m_datasetList;
};

#endif // DIALOGIMPORTNC_H
