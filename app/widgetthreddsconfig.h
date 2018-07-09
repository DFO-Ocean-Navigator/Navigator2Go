#ifndef WIDGETTHREDDSCONFIG_H
#define WIDGETTHREDDSCONFIG_H

#include "nodiscard.h"

#include <QWidget>

#include <optional>

#include <pugixml/pugixml.hpp>

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

	void on_pushButtonSaveConfig_clicked();

private:
	//
	void buildTable();
	//
	void checkCatalogsPath();
	//
	NODISCARD bool validateDatasetName(const QString& datasetName);
	//
	void createRow(const QString& datasetName);

	Ui::WidgetThreddsConfig* m_ui{nullptr};
	const Preferences* m_prefs{nullptr};

	std::optional<pugi::xml_document> m_catalogDoc;
};

#endif // WIDGETTHREDDSCONFIG_H
