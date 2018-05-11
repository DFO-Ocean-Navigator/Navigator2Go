#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "preferences.h"

#include <QMainWindow>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QHash>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}
class QListWidgetItem;
class QObject;

/***********************************************************************************/
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:

	// Menu callbacks
	void on_actionAbout_Qt_triggered();
	void on_actionOpen_triggered();
	void on_actionClose_triggered();
	void on_actionSave_triggered();
	void on_actionPreferences_triggered();
	void on_actionAbout_triggered();
	// Add Dataset button callback
	void on_buttonAddDataset_clicked();
	// Delete Dataset button callback
	void on_pushButtonDeleteDataset_clicked();
	// List item is double clicked
	void on_listWidgetConfigDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item);

	void on_tabWidget_currentChanged(int index);

	void on_pushButtonUpdateDoryList_clicked();

private:
	void readSettings();
	void writeSettings() const;
	void configureNetworkManager();
	void updateDoryDatasetList();

	Ui::MainWindow* m_ui;

	Preferences m_prefs;

	QString m_configFileName;
	QJsonObject m_documentRootObject;

	QNetworkAccessManager m_networkManager{this};

	bool m_hasUnsavedData{false};
	bool m_isNetworkConnected{true};

	QHash<QString, QString> m_doryDatasetNameToIDCache;
};

#endif // MAINWINDOW_H
