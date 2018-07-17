#ifndef WIDGETTHREDDSCONFIG_H
#define WIDGETTHREDDSCONFIG_H

#include "nodiscard.h"

#include <QWidget>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetThreddsConfig;
}

struct Preferences;

/***********************************************************************************/
class WidgetThreddsConfig : public QWidget {
	Q_OBJECT

public:
	WidgetThreddsConfig(QWidget* parent, const Preferences* prefs);

	~WidgetThreddsConfig();

private slots:
	void on_tableWidget_cellChanged(int row, int column);

	void on_pushButtonAddDataset_clicked();

	void on_pushButtonRemoveDataset_clicked();

private:
	//
	void buildTable();
	//
	void checkCatalogsPath();
	//
	NODISCARD bool validateDatasetName(const QString& datasetName);
	//
	void createRow(const QString& datasetName, const QString& dataPath);
	//
	void addDataset(const QString& datasetName, const QString& dataPath);
	//
	void removeDataset(const QString& datasetName, const QString& dataPath);

	Ui::WidgetThreddsConfig* m_ui{nullptr};
	const Preferences* const m_prefs{nullptr};
};

#endif // WIDGETTHREDDSCONFIG_H
