#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dialogdatasetview.h"
#include "dialogpreferences.h"
#include "apirequest.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QSaveFile>
#include <QSettings>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QNetworkReply>
#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
enum UITabs : int{
	HOME = 0,
	DATA_SYNC,
	CONFIG_EDITOR
};

/***********************************************************************************/
MainWindow::MainWindow(QWidget* parent) : 	QMainWindow{parent},
											m_ui{new Ui::MainWindow} {
	m_ui->setupUi(this);
	readSettings();

	setWindowTitle(tr("Ocean Navigator Dataset Config Editor"));

	// Set dark stylesheet
	QFile f{":qdarkstyle/style.qss"};
	if (!f.exists()) {
#ifdef QT_DEBUG
		qDebug() << "Unable to set stylesheet, file not found.";
#else
#endif
	}
	else {
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}

	configureNetworkManager();
	if (m_prefs.UpdateDoryListOnStart) {
		updateDoryDatasetList();
	}
	updateLocalDatasetList();

	m_ui->labelList->setText(tr("Double-click on a dataset to edit it's properties, or click on the Add Dataset button."));
	m_ui->labelList->setVisible(false);
	m_ui->buttonAddDataset->setText(tr("Add Dataset"));
	m_ui->buttonAddDataset->setEnabled(false);
	m_ui->pushButtonDeleteDataset->setText(tr("Delete Dataset"));
	m_ui->pushButtonDeleteDataset->setEnabled(false);
	m_ui->tabWidget->setCurrentIndex(UITabs::HOME);
}

/***********************************************************************************/
MainWindow::~MainWindow() {
	delete m_ui;
}

/***********************************************************************************/
void MainWindow::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this);
}

/***********************************************************************************/
void MainWindow::on_actionOpen_triggered() {
	QFileDialog dialog{this};
	dialog.setWindowTitle(tr("Open Dataset Config File"));
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Config Files (*.json)"));
	dialog.setViewMode(QFileDialog::Detail);
#ifdef QT_DEBUG
	dialog.setDirectory("/home/nabil/");
#else
	dialog.setDirectory("/opt/tools/");
#endif

	// Open file dialog
	if (dialog.exec()) {
		m_configFileName = dialog.selectedFiles().at(0); // Get selected file.
	}

	// If a file was actually selected (ignore cancel or close)
	if (!m_configFileName.isEmpty()) {

		// Open file
		QFile f{m_configFileName};
		f.open(QFile::ReadOnly | QFile::Text);
		const QString contents = f.readAll();
		f.close();

		// Parse json
		QJsonParseError error;
		const auto jsonDocument = QJsonDocument::fromJson(contents.toUtf8(), &error);
		// Check for errors
		if (jsonDocument.isNull()) {
			QMessageBox msgBox{this};
			msgBox.setText(tr("Error parsing JSON file."));
			msgBox.setInformativeText(error.errorString());
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();

			return;
		}
		m_documentRootObject = jsonDocument.object(); // Get copy of root object

		for (const auto& datasetName : m_documentRootObject.keys()) {
			m_ui->listWidgetConfigDatasets->addItem(datasetName);
		}

		m_ui->labelList->setVisible(true);
		m_ui->buttonAddDataset->setEnabled(true);
		m_ui->pushButtonDeleteDataset->setEnabled(true);
		m_ui->tabWidget->setCurrentIndex(UITabs::CONFIG_EDITOR);
		statusBar()->showMessage(tr("Config file loaded."), 1000);
	}
}

/***********************************************************************************/
void MainWindow::on_actionClose_triggered() {

	if (m_hasUnsavedData) {
		const auto reply = QMessageBox::question(this, tr("Confirm Action"), tr("Close without saving?"),
										QMessageBox::Yes | QMessageBox::Save | QMessageBox::Cancel);

		switch (reply) {
		case QMessageBox::Save:
			on_actionSave_triggered();
			break;
		case QMessageBox::Cancel:
			return;
		case QMessageBox::Yes:
		default:
			break;
		}
	}

	close();
}

/***********************************************************************************/
void MainWindow::on_actionSave_triggered() {

	if (!m_configFileName.isEmpty()) {
		const QJsonDocument doc{m_documentRootObject};

#ifdef QT_DEBUG
		QSaveFile f{"/home/nabil/dory.json"};
#else
		QSaveFile f(m_configFileName);
#endif
		f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate); // Overwrite original file.
		f.write(doc.toJson());
		f.commit();

		statusBar()->showMessage(tr("Config file saved!"), 1000);

		m_hasUnsavedData = false;
	}
}

/***********************************************************************************/
void MainWindow::on_buttonAddDataset_clicked() {
	m_hasUnsavedData = true;

	m_ui->listWidgetConfigDatasets->addItem("new_dataset_" + QString::number(qrand()));
}

/***********************************************************************************/
void MainWindow::on_listWidgetConfigDatasets_itemDoubleClicked(QListWidgetItem* item) {

	DialogDatasetView dialog{this};
	const auto datasetKey = item->text();
	dialog.SetData(datasetKey, m_documentRootObject[datasetKey].toObject());

	if (dialog.exec()) {
		const auto data = dialog.GetData();

		m_documentRootObject[datasetKey] = data.second;
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonDeleteDataset_clicked() {
	const auto reply = QMessageBox::question(this, tr("Confirm Action"), tr("Delete selected dataset(s)?"),
									QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		const auto items = m_ui->listWidgetConfigDatasets->selectedItems();

		for (auto* item : items) {
			delete m_ui->listWidgetConfigDatasets->takeItem(m_ui->listWidgetConfigDatasets->row(item));
		}
	}
}

/***********************************************************************************/
void MainWindow::closeEvent(QCloseEvent* event) {
	writeSettings();
}

/***********************************************************************************/
void MainWindow::readSettings() {
	QSettings settings{"OceanNavigator", "Config Tool"};

	settings.beginGroup("General");

	m_prefs.ONInstallDir = settings.value("ONInstallDir").toString();
	m_prefs.ONActiveDatasetConfig = settings.value("ONActiveDatasetConfig").toString();
	m_prefs.UpdateDoryListOnStart = settings.value("UpdateDoryListOnStart").toBool();

	settings.endGroup();
}

/***********************************************************************************/
void MainWindow::writeSettings() const {
	QSettings settings{"OceanNavigator", "Config Tool"};

	settings.beginGroup("General");

	settings.setValue("ONInstallDir", m_prefs.ONInstallDir);
	settings.setValue("ONActiveDatasetConfig", m_prefs.ONActiveDatasetConfig);
	settings.setValue("UpdateDoryListOnStart", m_prefs.UpdateDoryListOnStart);

	settings.endGroup();
}

/***********************************************************************************/
void MainWindow::configureNetworkManager() {
	m_networkManager.setRedirectPolicy(QNetworkRequest::RedirectPolicy::SameOriginRedirectPolicy);

}

/***********************************************************************************/
void MainWindow::updateDoryDatasetList() {
	statusBar()->showMessage(tr("Updating Dory dataset list"), 1000);

	const std::function<void(QJsonDocument)> replyHandler = [&](const auto& doc) {
		const auto root = doc.array();

		m_ui->listWidgetDoryDatasets->clear();
		m_datasetsAPIResultCache.clear();

		for (const auto& dataset : root) {
			const auto valueString = dataset["value"].toString();
			m_ui->listWidgetDoryDatasets->addItem(valueString);

			m_datasetsAPIResultCache.insert(valueString, dataset.toObject());
		}

		m_ui->pushButtonUpdateDoryList->setEnabled(true);
		m_ui->pushButtonUpdateDoryList->setText(tr("Update List"));

		this->statusBar()->showMessage(tr("Dory dataset list updated."), 1000);
	};

	m_ui->pushButtonUpdateDoryList->setEnabled(false);
	m_ui->pushButtonUpdateDoryList->setText(tr("Updating..."));

	MakeAPIRequest(m_networkManager, "http://navigator.oceansdata.ca/api/datasets/", replyHandler);
}

/***********************************************************************************/
void MainWindow::updateLocalDatasetList() {

}

/***********************************************************************************/
void MainWindow::on_actionPreferences_triggered() {
	DialogPreferences prefsDialog{this};

	prefsDialog.SetPreferences(m_prefs);

	if (prefsDialog.exec()) {
		m_prefs = prefsDialog.GetPreferences();

		updateLocalDatasetList();
	}
}

/***********************************************************************************/
void MainWindow::on_actionAbout_triggered() {
	QMessageBox::information(this, tr("About"), tr(""));
}

/***********************************************************************************/
void MainWindow::on_tabWidget_currentChanged(int index) {

	switch (index) {
	case UITabs::HOME:
		break;
	case UITabs::DATA_SYNC:
		break;
	case UITabs::CONFIG_EDITOR:
		break;
	default:
		break;
	}

}

/***********************************************************************************/
void MainWindow::on_pushButtonUpdateDoryList_clicked() {
	updateDoryDatasetList();
}

/***********************************************************************************/
void MainWindow::on_listWidgetDoryDatasets_itemDoubleClicked(QListWidgetItem* item) {
	const auto datasetID{m_datasetsAPIResultCache[item->text()]};

	DialogDatasetView dialog{this};
	dialog.SetData(datasetID, m_networkManager);

	if (dialog.exec()) {
	}
}

/***********************************************************************************/
void MainWindow::on_pushButtonDownload_clicked() {
	// Confirm download by showing selected values
	QMessageBox box{this};
	box.setWindowTitle(tr("Confirm Download?"));
	box.setIcon(QMessageBox::Question);

	if (box.exec()) {

	}
}
