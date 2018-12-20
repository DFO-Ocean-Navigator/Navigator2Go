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
class Preferences;

/***********************************************************************************/
/// Dataset config editor widget in the Config Editor tab.
class WidgetConfigEditor : public QWidget {
	Q_OBJECT

public:
	WidgetConfigEditor(QWidget* parent, const Preferences* prefs);
	~WidgetConfigEditor() override;

	void addDatasetToConfigList();
	void saveConfigFile();
	void updateDatasetListWidget();

	NODISCARD auto hasUnsavedData() const noexcept { return m_hasUnsavedData; }

signals:
	/// Signal emitted to MainWindow to show a given message in the window status bar.
	void showStatusBarMessage(const char* message);

private slots:
	/// Add Dataset button
	void on_pushButtonAddDataset_clicked();
	/// Delete Dataset button
	void on_pushButtonDeleteDataset_clicked();
	/// Save button
	void on_pushButtonSaveConfigFile_clicked();
	/// Load custom dataset config
	void on_pushButtonLoadCustomConfig_clicked();
	/// Load default dataset config
	void on_pushButtonLoadDefaultConfig_clicked();
	/// Double-click on list item.
	void on_listWidgetActiveDatasets_itemDoubleClicked(QListWidgetItem* item);

private:
	/// Load the default datasetconfigOFFLINE.json into the UI
	void setDefaultConfigFile();
	///
	int showUnsavedDataMessageBox();

	Ui::WidgetConfigEditor* m_ui{nullptr};			///< Pointer to UI widgets
	const Preferences* const m_prefs{nullptr};		///< Pointer to m_prefs in MainWindow.

	QString m_activeConfigFile;			///< Path to the currently loaded config file
	QJsonObject m_documentRootObject;	///< Root JSON object of active config file

	bool m_hasUnsavedData{false};		///< Does the current config file have unsaved data?
};

#endif // WIDGETCONFIGEDITOR_H
