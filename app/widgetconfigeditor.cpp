#include "widgetconfigeditor.h"
#include "ui_widgetconfigeditor.h"

#include "mainwindow.h"
#include "preferences.h"
#include "dialogdatasetview.h"
#include "jsonio.h"
#include "ioutils.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QFileDialog>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
WidgetConfigEditor::WidgetConfigEditor(QWidget* parent, const Preferences* prefs) :	QWidget{parent},
																													m_ui{new Ui::WidgetConfigEditor},
																													m_prefs{prefs} {
	m_ui->setupUi(this);

	m_ui->pushButtonAddDataset->setText(tr("Add Dataset"));
	m_ui->pushButtonDeleteDataset->setText(tr("Delete Dataset"));

	if (!prefs->AdvancedUI) {
		m_ui->pushButtonLoadCustomConfig->setVisible(false);
		m_ui->pushButtonLoadDefaultConfig->setVisible(false);
		m_ui->labelActiveConfigFile->setVisible(false);
	}

	const auto& offlineConfig{ m_prefs->ONInstallDir + QStringLiteral("/oceannavigator/datasetconfigOFFLINE.json") };
	if (!IO::FileExists(offlineConfig)) {
		IO::WriteJSONFile(offlineConfig);
	}

	setDefaultConfigFile();
}

/***********************************************************************************/
WidgetConfigEditor::~WidgetConfigEditor() {
	delete m_ui;
}

/***********************************************************************************/
void WidgetConfigEditor::addDatasetToConfigList() {
	bool ok;
	const auto name{ QInputDialog::getText(this,
										   tr("New dataset key"),
										   tr("Dataset key (e.g. giops_day):"),
										   QLineEdit::Normal,
										   QStringLiteral("new_dataset_") + QString::number(qrand()),
										   &ok
				   )};

	if (ok) {
		m_hasUnsavedData = true;
		m_ui->pushButtonSaveConfigFile->setEnabled(true);
		m_ui->listWidgetActiveDatasets->addItem(name);

		// Auto-open properties window
		const auto rowCount{ m_ui->listWidgetActiveDatasets->count() };
		on_listWidgetActiveDatasets_itemDoubleClicked(m_ui->listWidgetActiveDatasets->item(rowCount - 1));
	}
}

/***********************************************************************************/
void WidgetConfigEditor::saveConfigFile() {
	IO::WriteJSONFile(m_activeConfigFile, m_documentRootObject);

	m_hasUnsavedData = false;
	m_ui->pushButtonSaveConfigFile->setEnabled(false);

	emit showStatusBarMessage("Config file saved.");
}

/***********************************************************************************/
void WidgetConfigEditor::on_pushButtonAddDataset_clicked() {
	addDatasetToConfigList();
}

/***********************************************************************************/
void WidgetConfigEditor::on_pushButtonDeleteDataset_clicked() {
	const auto& items{ m_ui->listWidgetActiveDatasets->selectedItems() };

	if (!items.empty())	{
		const auto reply = QMessageBox::question(this, tr("Confirm Action"), tr("Delete selected dataset(s)?"),
												 QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			for (auto* item : items) {
				const auto& key{ m_documentRootObject.find(item->text()) };
				m_documentRootObject.erase(key);

				delete m_ui->listWidgetActiveDatasets->takeItem(m_ui->listWidgetActiveDatasets->row(item));
			}
		}
	}

	m_hasUnsavedData = true;
	m_ui->pushButtonSaveConfigFile->setEnabled(true);
}

/***********************************************************************************/
void WidgetConfigEditor::on_pushButtonSaveConfigFile_clicked() {
	saveConfigFile();
}

/***********************************************************************************/
void WidgetConfigEditor::on_pushButtonLoadCustomConfig_clicked() {

	if (m_hasUnsavedData) {
		switch(showUnsavedDataMessageBox()) {
		case QMessageBox::Yes:
			saveConfigFile();
			break;
		case QMessageBox::No:
			break;
		case QMessageBox::Cancel:
			return;
		default:
			break;
		}
	}

	const auto filePath{ QFileDialog::getOpenFileName(this,
													   tr("Select a dataset config file..."),
													   m_prefs->ONInstallDir,
													   QStringLiteral("Config Files (*.json)")
													   )
					   };

	if (filePath.isNull()) {
		return;
	}

	// Validate file
	const auto& doc = IO::LoadJSONFile(filePath);
	if (doc.isNull()) {
		return;
	}

	m_activeConfigFile = filePath;
	m_documentRootObject = doc.object();

	updateDatasetListWidget();

	emit showStatusBarMessage("Config file loaded.");
}

/***********************************************************************************/
void WidgetConfigEditor::on_pushButtonLoadDefaultConfig_clicked() {

	if (m_hasUnsavedData) {
		switch(showUnsavedDataMessageBox()) {
		case QMessageBox::Yes:
			saveConfigFile();
			break;
		case QMessageBox::No:
			break;
		case QMessageBox::Cancel:
			return;
		default:
			break;
		}
	}

	setDefaultConfigFile();
	updateDatasetListWidget();
}

/***********************************************************************************/
void WidgetConfigEditor::on_listWidgetActiveDatasets_itemDoubleClicked(QListWidgetItem* item) {
	DialogDatasetView dialog{this};
	const auto& datasetKey{ item->text() };
	dialog.SetData(datasetKey, m_documentRootObject[datasetKey].toObject());

	if (dialog.exec()) {
		const auto& data{ dialog.GetData() };

		m_documentRootObject[datasetKey] = data.second;

		m_hasUnsavedData = true;
		m_ui->pushButtonSaveConfigFile->setEnabled(true);
	}
}

/***********************************************************************************/
void WidgetConfigEditor::updateDatasetListWidget() {
	m_ui->listWidgetActiveDatasets->clear();

	const auto& keys{ m_documentRootObject.keys() };
	for (const auto& datasetName : keys) {
		m_ui->listWidgetActiveDatasets->addItem(datasetName);
	}

	m_ui->labelActiveConfigFile->setVisible(true);
	m_ui->labelActiveConfigFile->setText(tr("Active Config File: ") + QFileInfo(m_activeConfigFile).fileName());
	m_ui->pushButtonAddDataset->setEnabled(true);
	m_ui->pushButtonDeleteDataset->setEnabled(true);
}

/***********************************************************************************/
void WidgetConfigEditor::setDefaultConfigFile() {
	static const auto& offlineConfig{ m_prefs->ONInstallDir + QStringLiteral("/oceannavigator/datasetconfigOFFLINE.json") };

	// Validate file
	auto doc = IO::LoadJSONFile(offlineConfig);
	if (doc.isNull()) {
#ifdef QT_DEBUG
		qDebug() << "Config file not found: " << offlineConfig;
		qDebug() << "Creating it...";
#endif
		// File doesn't exist so create it
		IO::WriteJSONFile(offlineConfig);

		// And now load it
		doc = IO::LoadJSONFile(offlineConfig);

	}

	m_activeConfigFile = offlineConfig;
	m_documentRootObject = doc.object();

	emit showStatusBarMessage("Config file loaded.");
}

/***********************************************************************************/
int WidgetConfigEditor::showUnsavedDataMessageBox() {
	return QMessageBox::question(this,
								 tr("Confirm continue..."),
								 tr("You have unsaved changes. Do you want to save them?"),
								 QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel
								 );
}
