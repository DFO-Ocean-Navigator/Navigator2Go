#ifndef WIDGETTHREDDSCONFIG_H
#define WIDGETTHREDDSCONFIG_H

#include "nodiscard.h"

#include <QWidget>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetThreddsConfig;
}

class QString;

/***********************************************************************************/
/// Thredds config widget in the THREDDS Config Editor tab
class WidgetThreddsConfig : public QWidget {
	Q_OBJECT

public:
	WidgetThreddsConfig(QWidget* parent, const QString* threddsCatalogLocation);
	~WidgetThreddsConfig() override;

	/// Constructs the UI table
	void BuildTable();

private slots:
	void on_tableWidget_cellChanged(int row, int column);
	/// Add dataset button callback
	void on_pushButtonAddDataset_clicked();
	/// Remove dataset button callback
	void on_pushButtonRemoveDataset_clicked();
	/// Show THREDDS logs button callback: opens the log explorer
	void on_pushButtonShowLogs_clicked();
	/// Browse THREDDS directory button callback: opens the THREDDS content directory in the system's file explorer.
	void on_pushButtonBrowseTHREDDSDir_clicked();

private:
	/// Ensures the THREDDS catalog path is valid. Creates correct path if non-exisent.
	void checkCatalogsPath();
	/// Validates a given datasetName against illegal characters
	NODISCARD bool validateDatasetName(const QString& datasetName);
	/// Appends a row to the table widget given a datasetName and datapath
	void createRow(const QString& datasetName, const QString& dataPath);

	Ui::WidgetThreddsConfig* m_ui{nullptr};				///< Pointer to UI widgets
	const QString* const m_catalogLocation{nullptr};	///< Ptr to THREDDS catalog location in user settings
};

#endif // WIDGETTHREDDSCONFIG_H
