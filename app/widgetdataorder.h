#ifndef WIDGETDATAORDER_H
#define WIDGETDATAORDER_H

#include "QEasyDownloader.hpp"
#include "datadownloaddesc.h"

#include <QHash>
#include <QNetworkAccessManager>
#include <QWidget>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetDataOrder;
}
class QListWidgetItem;
class MainWindow;
class Preferences;

/***********************************************************************************/
/// Data order widget in the Data Order tab.
class WidgetDataOrder : public QWidget {
  Q_OBJECT

public:
  WidgetDataOrder(QWidget *parent, MainWindow *mainWindow, Preferences &prefs);
  ~WidgetDataOrder() override;

  ///
  void updateRemoteDatasetListWidget();

  void setNAMOnline();
  void setNAMOffline();

signals:
  /// Signal emitted to MainWindow to refresh the Ocean Navigator servers and
  /// cache.
  void refreshRequested();
  /// Signal emitted to MainWindow to show a given message in the window status
  /// bar.
  void showStatusBarMessage(const char *message);

private slots:

  /// Download button callback
  void on_pushButtonDownload_clicked();
  /// Remote dataset item double-clicked callback: Opens new window showing
  /// metadata
  void on_listWidgetRemoteDatasets_itemDoubleClicked(QListWidgetItem *item);
  /// Download queue item double-clicked callback: Removes item from queue
  void on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem *item);
  /// Update remote dataset widget button callback
  void on_pushButtonUpdateRemoteList_clicked();
  /// Delete button callback: removes selected item(s) from download queue.
  void on_pushButtonDeleteSelected_clicked();

  // Data order area changes
  void on_spinboxMinLat_valueChanged(double arg1);
  void on_spinboxMinLon_valueChanged(double arg1);
  void on_spinboxMaxLat_valueChanged(double arg1);
  void on_spinboxMaxLon_valueChanged(double arg1);

private:
  /// Setup m_downloader callbacks, and network settings
  void configureNetwork();
  /// Remove item from download queue given corresponding UI widget
  void deleteQueueItem(QListWidgetItem *item);

  Ui::WidgetDataOrder *m_ui{nullptr};      ///< Pointer to UI widgets
  MainWindow *const m_mainWindow{nullptr}; ///< Ptr to parent MainWindow.
  Preferences &m_prefs; ///< Ref to preferences object in MainWindow

  // Stores the resulting JSON objects for each dataset
  // returned by a call to:
  // http://navigator.oceansdata.ca/api/datasets/
  QHash<QString, QJsonObject>
      m_datasetsAPIResultCache; ///< Cache API results by dataset ID
  QHash<QString, DataDownloadDesc>
      m_downloadQueue; ///< Download queue maps dataset ID to download
                       ///< description

  // Network stuff
  QNetworkAccessManager m_networkAccessManager{this};
  QEasyDownloader m_downloader{this, &m_networkAccessManager};
};

#endif // WIDGETDATAORDER_H
