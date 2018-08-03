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
/// Dialog window to import a netCDF file.
class DialogImportNC : public QDialog {
	Q_OBJECT

public:
	DialogImportNC(const QString& threddsContentDir, QWidget* parent = nullptr);
	~DialogImportNC();

	/// Returns the list of files to import as NetCDFImportDesc.
	NODISCARD QVector<NetCDFImportDesc> GetImportList() const;

	/// Should the source file be removed?
	NODISCARD bool RemoveSourceNCFiles() const;

private slots:
	void on_pushButtonAdd_clicked();
	void on_pushButtonDelete_clicked();
	void on_tableWidget_doubleClicked(const QModelIndex& index);

private:
	///
	void addDataset();

	Ui::DialogImportNC* m_ui{nullptr};
	QStringList m_datasetList;
};

#endif // DIALOGIMPORTNC_H
