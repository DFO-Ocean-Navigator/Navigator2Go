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
struct Preferences;

/***********************************************************************************/
class WidgetDataOrder : public QWidget {
	Q_OBJECT

public:
	WidgetDataOrder(QWidget* parent, MainWindow* mainWindow, const Preferences* prefs);
	~WidgetDataOrder();

	//
	void updateRemoteDatasetListWidget();

	void setNAMOnline();
	void setNAMOffline();

	void setRegion(const double MinLat, const double MaxLat, const double MinLon, const double MaxLon);
	NODISCARD std::tuple<double, double, double, double> getRegion() const;

private slots:
	void on_pushButtonDownload_clicked();

	// List item is double clicked
	void on_listWidgetRemoteDatasets_itemDoubleClicked(QListWidgetItem* item);
	void on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem* item);

	void on_pushButtonUpdateRemoteList_clicked();

private:
	//
	void configureNetwork();

	Ui::WidgetDataOrder* m_ui{nullptr};
	MainWindow* const m_mainWindow{nullptr}; ///< Ptr to parent MainWindow.
	const Preferences* const m_prefs{nullptr}; ///< Ptr to m_prefs in MainWindow.

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
