#include "widgetdataorder.h"
#include "ui_widgetdataorder.h"

#include "dialogdatasetview.h"
#include "ioutils.h"
#include "mainwindow.h"
#include "network.h"
#include "systemutils.h"
#include "xmlio.h"

#include <QFileInfo>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QVector>

#ifdef QT_DEBUG
#include <QDebug>
#endif

/***********************************************************************************/
WidgetDataOrder::WidgetDataOrder(QWidget *parent, MainWindow *mainWindow,
                                 Preferences &prefs)
    : QWidget{parent}, m_ui{new Ui::WidgetDataOrder},
      m_mainWindow{mainWindow}, m_prefs{prefs} {
  m_ui->setupUi(this);

  m_ui->groupBoxDownloadStats->setVisible(false);
  m_ui->spinboxMinLat->setValue(m_prefs.DataOrderArea.MinLat);
  m_ui->spinboxMaxLat->setValue(m_prefs.DataOrderArea.MaxLat);

  m_ui->spinboxMinLon->setValue(m_prefs.DataOrderArea.MinLon);
  m_ui->spinboxMaxLon->setValue(m_prefs.DataOrderArea.MaxLon);

  configureNetwork();
}

/***********************************************************************************/
WidgetDataOrder::~WidgetDataOrder() { delete m_ui; }

/***********************************************************************************/
void WidgetDataOrder::on_pushButtonUpdateRemoteList_clicked() {
  updateRemoteDatasetListWidget();
}

/***********************************************************************************/
void WidgetDataOrder::on_pushButtonDownload_clicked() {
  m_ui->listWidgetDownloadQueue->selectAll();
  const auto &items{m_ui->listWidgetDownloadQueue->selectedItems()};

  if (items.empty()) {
    QMessageBox::information(
        this, tr("Download queue empty"),
        tr("Your download queue is empty! Add some stuff to download!"));
    return;
  }

  if (m_prefs.THREDDSCatalogLocation.isEmpty()) {
    QMessageBox::information(this, tr("THREDDS location not set..."),
                             tr("Go to Preferences (CTRL + SHIFT + P) to set "
                                "the location on disk."));
    return;
  }

  QMessageBox box{this};
  box.setWindowTitle(tr("Confirm Download?"));
  box.setText(tr("Depending on your network connection speed, the download "
                 "could take a significant period of time to complete."));
  box.setIcon(QMessageBox::Question);
  box.setStandardButtons(QMessageBox::StandardButton::Cancel |
                         QMessageBox::StandardButton::Ok);

  if (box.exec() == QMessageBox::StandardButton::Ok) {
    // Disable buttons
    m_ui->pushButtonUpdateRemoteList->setEnabled(false);
    m_ui->pushButtonDownload->setEnabled(false);

    // Get given region of interest
    const QString &min_range{QStringLiteral("&min_range=") +
                             QString::number(m_ui->spinboxMinLat->value()) +
                             QStringLiteral(",") +
                             QString::number(m_ui->spinboxMinLon->value())};
    const QString &max_range{QStringLiteral("&max_range=") +
                             QString::number(m_ui->spinboxMaxLat->value()) +
                             QStringLiteral(",") +
                             QString::number(m_ui->spinboxMaxLon->value())};

    // Check if each dataset in the queue has a catalog file
    // and storage folder before downloading.
    QVector<QString> downloadPaths;
    for (const auto &item : m_downloadQueue) {

      auto fileDesc{IO::GetNCFilename(m_prefs.THREDDSCatalogLocation, item)};

      if (fileDesc.Path.isEmpty()) {

        // Dataset doesn't exist locally so create a catalog and folder for it
        const auto &location{QStringLiteral("/opt/thredds_content/data/") +
                             item.ID};
        IO::addDataset(m_prefs.THREDDSCatalogLocation, item.ID, location);

        fileDesc.Path = location;
      }
      downloadPaths.push_back(fileDesc.Path + fileDesc.Filename);
    }

    // Show download stats in UI
    m_ui->groupBoxDownloadStats->setVisible(true);
    // Show progress bar in UI
    m_mainWindow->showProgressBar("Download Progress: ");

    // Tell downloader to download the files now.
    int i{0};
    for (const auto &item : m_downloadQueue) {
      const auto &url{item.GetAPIQuery(m_prefs.RemoteURL) + min_range +
                      max_range + QStringLiteral("&output_format=") +
                      m_prefs.DataDownloadFormat};

      m_downloader.Download(url, downloadPaths[i]);

      ++i;
    }
  }
}

/***********************************************************************************/
void WidgetDataOrder::on_listWidgetRemoteDatasets_itemDoubleClicked(
    QListWidgetItem *item) {
  const auto &datasetID{m_datasetsAPIResultCache[item->text()]};
  DialogDatasetView dialog{this};

  auto isUpdatingDownload{false};
  if (m_downloadQueue.find(item->text()) != m_downloadQueue.end()) {
    isUpdatingDownload = true;
  }

  dialog.SetData(datasetID, m_networkAccessManager);

  if (dialog.exec()) {
    // Only add to queue if variables were selected.
    if (const auto &data{dialog.GetDownloadData()};
        !data.SelectedVariables.empty()) {

      // Don't accept a giant date range
      std::size_t dayLimit{60};
      if (data.Quantum == QStringLiteral("month")) {
        dayLimit = 1825; // 5 years of monthly data only
      }

      if (static_cast<std::size_t>(data.StartDate.daysTo(data.EndDate)) >
          dayLimit) {
        QMessageBox box{this};
        box.setWindowTitle(tr("Selected date range was too large..."));
        box.setText(tr("For datasets with quantum \"day\" and \"hour\", 60 "
                       "days is the limit. \nFor datasets with quantum "
                       "\"month\", 5 years is the limit."));
        box.setIcon(QMessageBox::Critical);
        box.setStandardButtons(QMessageBox::StandardButton::Ok);

        box.exec();

        return;
      }

      if (!isUpdatingDownload) {
        m_ui->listWidgetDownloadQueue->addItem(data.Name);
      }
      m_downloadQueue.insert(data.Name, data);
    }
  }
}

/***********************************************************************************/
void WidgetDataOrder::on_listWidgetDownloadQueue_itemDoubleClicked(
    QListWidgetItem *item) {

  deleteQueueItem(item);
}

/***********************************************************************************/
void WidgetDataOrder::on_spinboxMinLat_valueChanged(double arg1) {
  m_prefs.DataOrderArea.MinLat = arg1;
}

/***********************************************************************************/
void WidgetDataOrder::on_spinboxMinLon_valueChanged(double arg1) {
  m_prefs.DataOrderArea.MinLon = arg1;
}

/***********************************************************************************/
void WidgetDataOrder::on_spinboxMaxLat_valueChanged(double arg1) {
  m_prefs.DataOrderArea.MaxLat = arg1;
}

/***********************************************************************************/
void WidgetDataOrder::on_spinboxMaxLon_valueChanged(double arg1) {
  m_prefs.DataOrderArea.MaxLon = arg1;
}

/***********************************************************************************/
void WidgetDataOrder::configureNetwork() {
  // Follow server redirects for same domain only
  m_networkAccessManager.setRedirectPolicy(
      QNetworkRequest::RedirectPolicy::SameOriginRedirectPolicy);

  // Reduce latency by connecting to remote first
  m_networkAccessManager.connectToHost(m_prefs.RemoteURL);

  m_downloader.setTimeoutTime(300000); // 30 sec timeout

  // Configure downloader
#ifdef QT_DEBUG
  m_downloader.setDebug(true);
  QObject::connect(&m_downloader, &QEasyDownloader::Debugger, this,
                   [&](const auto &msg) { qDebug() << msg; });
#endif

  // Full Download Progress. Emitted on every download.
  QObject::connect(
      &m_downloader, &QEasyDownloader::DownloadProgress, this,
      [&](const auto bytesReceived, const auto total, const auto percent,
          const auto speed, const auto &unit, const auto &url,
          const auto &filename) {
        m_ui->labelCurrentFile->setText(QFileInfo(filename).fileName());
        m_ui->labelDownSpeedValue->setText(QString::number(speed) + " " + unit);
        m_ui->labelFileSize->setText(QString::number(total >> 20) +
                                     QStringLiteral(" MB")); // Bytes to MB
        m_mainWindow->updateProgressBar(percent);
      });

  // Emitted when a single file is downloaded.
  if (!QObject::connect(&m_downloader, &QEasyDownloader::DownloadFinished, this,
                        [&](const auto &url, const auto &filename) {})) {
  }

  // Emitted when all jobs are done.
  QObject::connect(&m_downloader, &QEasyDownloader::Finished, this, [&]() {
    m_ui->listWidgetDownloadQueue->clear();
    m_ui->pushButtonDownload->setEnabled(true);
    m_ui->pushButtonUpdateRemoteList->setEnabled(true);
    this->m_mainWindow->hideProgressBar();
    m_downloadQueue.clear();
    m_ui->groupBoxDownloadStats->setVisible(false);

    System::SendDesktopNotification(QStringLiteral("Navigator2Go"),
                                    QStringLiteral("All downloads complete!"));

    emit refreshRequested();
  });

  // Emitted on error.
  QObject::connect(
      &m_downloader, &QEasyDownloader::Error, this,
      [&](const auto &errorCode, const auto &url, const auto &filename) {
#ifdef QT_DEBUG
        qDebug() << "Error: " << errorCode << " " << url;
#endif
        QMessageBox box{this};
        box.setIcon(QMessageBox::Critical);
        box.setWindowTitle(tr("Download error..."));
        box.setText(
            tr("A error has occoured while downloading your file. If it "
               "exists, we will continue with the next file in your queue."));
        box.setDetailedText(
            url.toString() +
            QStringLiteral("\n\n QNetworkReply::NetworkError: ") +
            QVariant(errorCode).toString() +
            QStringLiteral(" http://doc.qt.io/archives/qt-4.8/"
                           "qnetworkreply.html#NetworkError-enum"));

        box.exec();

        // Move on to next file in queue.
        if (m_downloader.HasNext()) {
#ifdef QT_DEBUG
          qDebug() << "Moving on to next file";
#endif
          m_downloader.Next();
        } else {
          m_ui->listWidgetDownloadQueue->clear();
          this->m_mainWindow->hideProgressBar();
          m_ui->pushButtonUpdateRemoteList->setEnabled(true);
          m_ui->pushButtonDownload->setEnabled(true);
          m_ui->groupBoxDownloadStats->setVisible(false);

          m_downloadQueue.clear();
        }
      });

  // Emitted when there is a timeout.
  QObject::connect(&m_downloader, &QEasyDownloader::Timeout, this,
                   [&](const auto &url, const auto &filename) {
                     QMessageBox box{this};
                     box.setWindowTitle(tr("Download has timed out..."));
                     box.setInformativeText("URL: " + url.toString());
                     box.setDetailedText(filename);
                     box.setIcon(QMessageBox::Warning);

                     box.exec();

                     if (m_downloader.HasNext()) {
                       m_downloader.Next();
                     }
                   });
}

/***********************************************************************************/
void WidgetDataOrder::deleteQueueItem(QListWidgetItem *item) {
  // Remove from queue table
  m_downloadQueue.remove(item->text());

  // Remove from UI
  delete m_ui->listWidgetDownloadQueue->takeItem(
      m_ui->listWidgetDownloadQueue->row(item));
}

/***********************************************************************************/
void WidgetDataOrder::on_pushButtonDeleteSelected_clicked() {

  // Get selected items in queue widget
  const auto &selected{m_ui->listWidgetDownloadQueue->selectedItems()};
  if (selected.empty()) { // return if nothing selected
    return;
  }
  for (const auto &item : selected) {
    deleteQueueItem(item);
  }
}

/***********************************************************************************/
void WidgetDataOrder::updateRemoteDatasetListWidget() {
  emit showStatusBarMessage("Updating remote dataset list...");

  m_ui->pushButtonUpdateRemoteList->setEnabled(false);
  m_ui->pushButtonUpdateRemoteList->setText(tr("Updating..."));

  Network::MakeAPIRequest(
      m_networkAccessManager,
      m_prefs.RemoteURL + QStringLiteral("/api/datasets/"),
      // Success handler
      [&](const auto &doc) {
        const auto &root = doc.array();

        m_ui->listWidgetRemoteDatasets->clear();
        m_datasetsAPIResultCache.clear();
        for (const auto &dataset : root) {
          const auto &valueString = dataset["value"].toString();
          m_ui->listWidgetRemoteDatasets->addItem(valueString);

          m_datasetsAPIResultCache.insert(valueString, dataset.toObject());
        }

        m_ui->pushButtonUpdateRemoteList->setEnabled(true);
        m_ui->pushButtonUpdateRemoteList->setText(tr("Update List"));

        emit showStatusBarMessage("Remote dataset list updated.");
      },
      // Error handler
      [&]() {
        m_ui->pushButtonUpdateRemoteList->setEnabled(true);
        m_ui->pushButtonUpdateRemoteList->setText(tr("Update List"));

        emit showStatusBarMessage("Failed to update remote dataset list.");
      });
}

/***********************************************************************************/
void WidgetDataOrder::setNAMOnline() {
  m_networkAccessManager.setNetworkAccessible(
      QNetworkAccessManager::Accessible);
}

/***********************************************************************************/
void WidgetDataOrder::setNAMOffline() {
  m_networkAccessManager.setNetworkAccessible(
      QNetworkAccessManager::NotAccessible);
}
