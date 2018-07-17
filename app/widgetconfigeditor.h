#ifndef WIDGETCONFIGEDITOR_H
#define WIDGETCONFIGEDITOR_H

#include "nodiscard.h"

#include <QWidget>
#include <QJsonObject>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetConfigEditor;
}

class QListWidgetItem;
class MainWindow;
struct Preferences;

/***********************************************************************************/
class WidgetConfigEditor : public QWidget {
	Q_OBJECT

public:
	WidgetConfigEditor(QWidget* parent, const MainWindow* mainWindow, const Preferences* prefs);
	~WidgetConfigEditor();

	void addDatasetToConfigList();
	void saveConfigFile();
	void updateDatasetListWidget();
	void setDefaultConfigFile();

	NODISCARD auto hasUnsavedData() const noexcept { return m_hasUnsavedData; }

private slots:

	// Add Dataset button
	void on_pushButtonAddDataset_clicked();
	// Delete Dataset button
	void on_pushButtonDeleteDataset_clicked();
	// Save button
	void on_pushButtonSaveConfigFile_clicked();
	// Load custom dataset config
	void on_pushButtonLoadCustomConfig_clicked();
	// Load default dataset config
	void on_pushButtonLoadDefaultConfig_clicked();

	void on_listWidgetActiveDatasets_itemDoubleClicked(QListWidgetItem* item);

private:
	//
	int showUnsavedDataMessageBox();

	Ui::WidgetConfigEditor* m_ui{nullptr};
	const MainWindow* const m_mainWindow{nullptr};
	const Preferences* const m_prefs{nullptr};

	// Path to the currently loaded config file
	QString m_activeConfigFile;
	// Root JSON object of active config file
	QJsonObject m_documentRootObject;

	bool m_hasUnsavedData{false};
};

#endif // WIDGETCONFIGEDITOR_H
