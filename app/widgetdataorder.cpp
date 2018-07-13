#include "widgetdataorder.h"
#include "ui_widgetdataorder.h"

#include "mainwindow.h"
#include "dialogdatasetview.h"
#include "ioutils.h"
#include "network.h"

#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>

/***********************************************************************************/
WidgetDataOrder::WidgetDataOrder(QWidget* parent, MainWindow* mainWindow, const Preferences* prefs) :	QWidget{parent},
																										m_ui{new Ui::WidgetDataOrder},
																										m_mainWindow{mainWindow},
																										m_prefs{prefs} {
	m_ui->setupUi(this);

	configureNetwork();
}

/***********************************************************************************/
WidgetDataOrder::~WidgetDataOrder() {
	delete m_ui;
}

/***********************************************************************************/
void WidgetDataOrder::setRegion(const double MinLat, const double MaxLat, const double MinLon, const double MaxLon) {
	m_ui->spinboxMinLat->setValue(MinLat);
	m_ui->spinboxMaxLat->setValue(MaxLat);

	m_ui->spinboxMinLon->setValue(MinLon);
	m_ui->spinboxMaxLon->setValue(MaxLon);
}

/***********************************************************************************/
std::tuple<double, double, double, double> WidgetDataOrder::getRegion() const {
	return std::make_tuple(	m_ui->spinboxMinLat->value(),
							m_ui->spinboxMaxLat->value(),
							m_ui->spinboxMinLon->value(),
							m_ui->spinboxMaxLon->value()
							);
}

/***********************************************************************************/
void WidgetDataOrder::on_pushButtonUpdateRemoteList_clicked() {
	updateRemoteDatasetListWidget();
}

/***********************************************************************************/
void WidgetDataOrder::on_pushButtonDownload_clicked() {
	m_ui->listWidgetDownloadQueue->selectAll();
	const auto& items{ m_ui->listWidgetDownloadQueue->selectedItems() };

	if (items.empty()) {
		QMessageBox::information(this, tr("Download queue empty"), tr("Your download queue is empty! Add some stuff to download!"));
		return;
	}

	if (m_prefs->THREDDSCatalogLocation.isEmpty()) {
		QMessageBox::information(this, tr("THREDDS location not set..."), tr("Go to Preferences (CTRL + SHIFT + P) to set the location on disk."));
		return;
	}

	QMessageBox box{this};
	box.setWindowTitle(tr("Confirm Download?"));
	box.setText(tr("Depending on your network connection speed, the download could take a significant period of time to complete."));
	box.setIcon(QMessageBox::Question);
	box.setStandardButtons(QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Ok);

	if (box.exec() == QMessageBox::StandardButton::Ok) {
		// Disable buttons
		m_ui->pushButtonUpdateRemoteList->setEnabled(false);
		m_ui->pushButtonDownload->setEnabled(false);

		// Get given region of interest
		const QString min_range{ "&min_range=" + QString::number(m_ui->spinboxMinLat->value()) + "," + QString::number(m_ui->spinboxMinLon->value()) };
		const QString max_range{ "&max_range=" + QString::number(m_ui->spinboxMaxLat->value()) + "," + QString::number(m_ui->spinboxMaxLon->value()) };

		for (const auto& item : m_downloadQueue) {
			const auto& url{ item.ToAPIURL() + min_range + max_range + "&output_format=" + m_prefs->DataDownloadFormat};
#ifdef QT_DEBUG
			qDebug() << IO::FindPathForDataset(m_prefs->THREDDSCatalogLocation, item);
#endif

			m_downloader.Download(url, IO::FindPathForDataset(m_prefs->THREDDSCatalogLocation, item) + ".nc");
		}

		// Show download stuff
		m_mainWindow->showProgressBar("Download Progress: ");
	}
}

/***********************************************************************************/
void WidgetDataOrder::on_listWidgetRemoteDatasets_itemDoubleClicked(QListWidgetItem* item) {
	const auto& datasetID{m_datasetsAPIResultCache[item->text()]};
	DialogDatasetView dialog{this};

	auto isUpdatingDownload{false};
	if (m_downloadQueue.find(item->text()) != m_downloadQueue.end()) {
		isUpdatingDownload = true;
	}

	dialog.SetData(datasetID, m_networkAccessManager);

	if (dialog.exec()) {
		const auto& data{ dialog.GetDownloadData() };
		// Only add to queue if variables were selected.
		if (!data.SelectedVariables.empty()) {

			// Don't accept a giant date range
			std::size_t dayLimit{ 60 };
			if (m_datasetsAPIResultCache[data.ID]["quantum"] == "month") {
				dayLimit = 1825; // 5 years of monthly data only
			}

			if (static_cast<std::size_t>(data.StartDate.daysTo(data.EndDate)) > dayLimit) {
				QMessageBox box{this};
				box.setWindowTitle(tr("Selected date range was too large..."));
				box.setText(tr("For datasets with quantum \"day\" and \"hour\", 60 days is the limit. \nFor datasets with quantum \"month\", 5 years is the limit."));
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
void WidgetDataOrder::on_listWidgetDownloadQueue_itemDoubleClicked(QListWidgetItem* item) {
	delete m_ui->listWidgetDownloadQueue->takeItem(m_ui->listWidgetDownloadQueue->row(item));
}

/***********************************************************************************/
void WidgetDataOrder::configureNetwork() {
	// Follow server redirects for same domain only
	m_networkAccessManager.setRedirectPolicy(QNetworkRequest::RedirectPolicy::SameOriginRedirectPolicy);

	if (m_prefs->IsNetworkOnline) {
		// Reduce latency by connecting to remote first
		m_networkAccessManager.connectToHost(m_prefs->RemoteURL);
	}

	m_downloader.setTimeoutTime(300000); // 30 sec timeout

	// Configure downloader
#ifdef QT_DEBUG
	m_downloader.setDebug(true);
	QObject::connect(&m_downloader, &QEasyDownloader::Debugger, this,
					 [&](const auto& msg) {
						qDebug() << msg;
						return;
					}
	);
#endif

	// Full Download Progress. Emitted on every download.
	QObject::connect(&m_downloader, &QEasyDownloader::DownloadProgress, this,
					 [&](const auto bytesReceived, const auto percent, const auto speed, const auto& unit, const auto& url, const auto& filename) {
#ifdef QT_DEBUG
#endif
					}
	);

	// Emitted when a single file is downloaded.
	QObject::connect(&m_downloader, &QEasyDownloader::DownloadFinished, this,
					 [&](const auto& url, const auto& filename) {
						this->m_mainWindow->showStatusBarMessage("File downloaded.");

						++this->m_numDownloadsComplete;
						const auto percent{ 100 * (this->m_numDownloadsComplete / static_cast<std::size_t>(this->m_downloadQueue.size())) };
						this->m_mainWindow->updateProgressBar(static_cast<int>(percent));

						if (percent == 100) {
							// Prevent divide-by-zero if this is in the below lambda
							m_downloadQueue.clear();
						}
					}
	);

	// Emitted when all jobs are done.
	QObject::connect(&m_downloader, &QEasyDownloader::Finished, this,
					 [&]() {
#ifdef QT_DEBUG
						qDebug() << "All downloads complete";
#endif
						m_ui->pushButtonUpdateAggConfig->setEnabled(true);
						m_ui->listWidgetDownloadQueue->clear();
						m_ui->pushButtonDownload->setEnabled(true);
						this->m_mainWindow->hideProgressBar();

						QMessageBox box{this};
						box.setWindowTitle(tr("Downloads completed..."));
						box.setText(tr("All downloads completed! Your queue has been emptied."));
						box.setInformativeText(tr("Be sure to click the button below to update your config and aggregation files!"));
						box.setIcon(QMessageBox::Information);

						box.exec();
					}
	);

	// Emitted on error.
	QObject::connect(&m_downloader, &QEasyDownloader::Error, this,
					 [&](const auto& errorCode, const auto& url, const auto& filename) {
#ifdef QT_DEBUG
						qDebug() << "Error: " << errorCode << " " << url;
#endif
						++this->m_numDownloadsComplete;
						const auto percent{ 100 * (this->m_numDownloadsComplete / static_cast<std::size_t>(this->m_downloadQueue.size())) };
						this->m_mainWindow->updateProgressBar(static_cast<int>(percent));

						QMessageBox box{this};
						box.setIcon(QMessageBox::Critical);
						box.setWindowTitle(tr("Download error..."));
						box.setText(tr("A error has occoured while downloading your file. If it exists, we will continue with the next file in your queue."));
						box.setDetailedText( url.toString() + "\n\n QNetworkReply::NetworkError: " + QVariant(errorCode).toString() +
											" http://doc.qt.io/archives/qt-4.8/qnetworkreply.html#NetworkError-enum");

						box.exec();

						// Move on to next file in queue.
						if (m_downloader.HasNext()) {
#ifdef QT_DEBUG
							qDebug() << "Moving on to next file";
#endif
							m_downloader.Next();
						}
						else {
							m_ui->pushButtonUpdateAggConfig->setEnabled(true);
							m_ui->listWidgetDownloadQueue->clear();
							this->m_mainWindow->hideProgressBar();

							m_downloadQueue.clear();
						}
					}
	);

	// Emitted when there is a timeout.
	QObject::connect(&m_downloader, &QEasyDownloader::Timeout, this,
					 [&](const auto& url, const auto& filename) {
						QMessageBox box{this};
						box.setWindowTitle(tr("Download has timed out..."));
						box.setInformativeText("URL: " + url.toString());
						box.setDetailedText(filename);

						box.setIcon(QMessageBox::Warning);

						box.exec();
					}
	);
}

/***********************************************************************************/
void WidgetDataOrder::updateRemoteDatasetListWidget() {
	m_mainWindow->showStatusBarMessage("Updating remote dataset list...");

	m_ui->pushButtonUpdateRemoteList->setEnabled(false);
	m_ui->pushButtonUpdateRemoteList->setText(tr("Updating..."));

	Network::MakeAPIRequest(m_networkAccessManager, m_prefs->RemoteURL+"/api/datasets/",
							// Success handler
							[&](const auto& doc) {
								const auto& root = doc.array();

								m_ui->listWidgetRemoteDatasets->clear();
								m_datasetsAPIResultCache.clear();
								for (const auto& dataset : root) {
									const auto& valueString = dataset["value"].toString();
									m_ui->listWidgetRemoteDatasets->addItem(valueString);

									m_datasetsAPIResultCache.insert(valueString, dataset.toObject());
								}

								m_ui->pushButtonUpdateRemoteList->setEnabled(true);
								m_ui->pushButtonUpdateRemoteList->setText(tr("Update List"));

								this->m_mainWindow->showStatusBarMessage("Remote dataset list updated.");
							},
							// Error handler
							[&]() {
								m_ui->pushButtonUpdateRemoteList->setEnabled(true);
								m_ui->pushButtonUpdateRemoteList->setText(tr("Update List"));

								this->m_mainWindow->showStatusBarMessage("Failed to update remote dataset list.");
							}
	);
}

/***********************************************************************************/
void WidgetDataOrder::setNAMOnline() {
	m_networkAccessManager.setNetworkAccessible(QNetworkAccessManager::Accessible);
}

/***********************************************************************************/
void WidgetDataOrder::setNAMOffline() {
	m_networkAccessManager.setNetworkAccessible(QNetworkAccessManager::NotAccessible);
}
