#ifndef WIDGETDATAORDER_H
#define WIDGETDATAORDER_H

#include "QEasyDownloader.hpp"
#include "datadownloaddesc.h"

#include <QWidget>
#include <QHash>
#include <QNetworkAccessManager>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetDataOrder;
}
class QListWidgetItem;
class MainWindow;
class Preferences;

/***********************************************************************************/
class WidgetDataOrder : public QWidget {
	Q_OBJECT

public:
	WidgetDataOrder(QWidget* parent, MainWindow* mainWindow, Preferences& prefs);
	~WidgetDataOrder() override;

	///
	void updateRemoteDatasetListWidget();

	void setNAMOnline();
	void setNAMOffline();

signals:
	///
	void refreshRequested();
	///
	void showStatusBarMessage(const char* message);

private slots:
	void on_pushButtonDownload_clicked();

	// List item is double clicked
	void on_listWidgetRemoteDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem* item);

	void on_pushButtonUpdateRemoteList_clicked();

	// Data order area changes
	void on_spinboxMinLat_valueChanged(double arg1);
	void on_spinboxMinLon_valueChanged(double arg1);
	void on_spinboxMaxLat_valueChanged(double arg1);
	void on_spinboxMaxLon_valueChanged(double arg1);

	void on_pushButtonDeleteSelected_clicked();

private:
	//
	void configureNetwork();
	//
	void deleteQueueItem(QListWidgetItem* item);

	Ui::WidgetDataOrder* m_ui{nullptr};
	MainWindow* const m_mainWindow{nullptr}; ///< Ptr to parent MainWindow.
	Preferences& m_prefs;

	// Stores the resulting JSON objects for each dataset
	// returned by a call to:
	// http://navigator.oceansdata.ca/api/datasets/
	QHash<QString, QJsonObject> m_datasetsAPIResultCache;
	QHash<QString, DataDownloadDesc> m_downloadQueue;

	// Network stuff
	QNetworkAccessManager m_networkAccessManager{this};
	QEasyDownloader m_downloader{this, &m_networkAccessManager};
};

#endif // WIDGETDATAORDER_H
