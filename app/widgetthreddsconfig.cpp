#include "widgetthreddsconfig.h"
#include "ui_widgetthreddsconfig.h"

#include "preferences.h"
#include "constants.h"

#include <QFile>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>
#include <QInputDialog>
#include <QRegularExpression>

#include <cstring>
#include <optional>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

/***********************************************************************************/
NODISCARD auto readXML(const QString& path) {
	pugi::xml_document doc;

	const auto result{ doc.load_file(path.toStdString().c_str()) };

	return result ? std::make_optional(std::move(doc)) : std::nullopt;
}

/***********************************************************************************/
WidgetThreddsConfig::WidgetThreddsConfig(QWidget* parent, const Preferences* prefs) :	QWidget{parent},
																						m_ui{new Ui::WidgetThreddsConfig},
																						m_prefs{prefs} {
	m_ui->setupUi(this);

	m_ui->tableWidget->horizontalHeader()->setStretchLastSection(true); // Resize columns to widget width
	m_ui->tableWidget->setHorizontalHeaderLabels({
													tr("Dataset Name"),
													tr("Catalog Path"),
													tr("Dataset Files Location")
												 });

	m_ui->labelTHREDDSPath->setText(tr("THREDDS Location: ") + m_prefs->THREDDSCatalogLocation);
	m_ui->labelTHREDDSPath->setStyleSheet(QSS_COLOR_GREEN);

	checkCatalogsPath();

	buildTable();
}

/***********************************************************************************/
WidgetThreddsConfig::~WidgetThreddsConfig() {
	delete m_ui;
}

/***********************************************************************************/
void WidgetThreddsConfig::on_tableWidget_cellChanged(int row, int column) {
	switch (column) {
	case 0:
		if (validateDatasetName(m_ui->tableWidget->item(row, 0)->text())) {
			m_ui->tableWidget->item(row, 0)->setTextColor(QColor(Qt::white));
		}
		else {
			m_ui->tableWidget->item(row, 0)->setTextColor(QColor(COLOR_RED));
		}
		break;
	default:
		break;
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::on_pushButtonAddDataset_clicked() {
	bool ok;
	const auto datasetName { QInputDialog::getText(	this,
													tr("Add THREDDS Dataset..."),
													tr("Enter a dataset name.\nNo whitespace, /, \\, +, *, ., characters allowed."),
													QLineEdit::Normal,
													"my_dataset_" + QString::number(qrand()),
													&ok)
						   };

	if (ok && validateDatasetName(datasetName)) {
		createRow(datasetName);
		return;
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::on_pushButtonRemoveDataset_clicked() {
	const auto currentRow{ m_ui->tableWidget->currentRow() };

	if (currentRow != -1) {

		const auto reply{ QMessageBox::question(this, tr("Confirm Action"), tr("Delete selected dataset?"),
												 QMessageBox::Yes | QMessageBox::No)
						};

		if (reply == QMessageBox::Yes) {
			m_ui->tableWidget->removeRow(m_ui->tableWidget->currentRow());
		}
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::on_pushButtonSaveConfig_clicked() {

	QMessageBox::information(this,
							 tr("Success..."),
							 tr("THREDDS config successfully updated! Please restart the THREDDS server from the Dashboard."));
}

/***********************************************************************************/
void WidgetThreddsConfig::buildTable() {

	const auto catalogFile{ m_prefs->THREDDSCatalogLocation + QString("/catalog.xml") };

	m_catalogDoc = readXML(catalogFile);

	if (!m_catalogDoc.has_value()) {
		QMessageBox box{this};
		box.setWindowTitle(tr("THREDDS catalog error..."));
		box.setText(tr("Failed to load THREDDS catalog file: ") + catalogFile);
		box.setIcon(QMessageBox::Critical);

		box.exec();

		return;
	}

	const auto rootNode{ m_catalogDoc->child("catalog") };

	if (!rootNode.select_node("catalogRef")) {
		// Check if the primary catalog doesn't have any dataset references
		return;
	}

	for (const auto& node : rootNode.children()) {
		// Resort to manual comparison since pugi is only listing the first catalog
		// when using node.next_sibling() *shrug*
		if (std::strcmp(node.name(), "catalogRef") == 0) {

			m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());
			const auto rowIdx{ m_ui->tableWidget->rowCount() - 1 };

			auto* nameItem{ new QTableWidgetItem(node.attribute("xlink:title").as_string()) };

			const auto catalogPath{ node.attribute("xlink:href").as_string() };
			auto* pathItem{ new QTableWidgetItem(catalogPath) };
			pathItem->setFlags(pathItem->flags() ^ Qt::ItemIsEditable); // Make read-only

			m_ui->tableWidget->setItem(rowIdx, 0, nameItem);
			m_ui->tableWidget->setItem(rowIdx, 1, pathItem);


			const auto datasetCatalog{ readXML(m_prefs->THREDDSCatalogLocation + "/" + catalogPath) };
			if (!datasetCatalog.has_value()) {
				continue;
			}

			const auto datasetScanNode{ datasetCatalog->child("catalog").child("datasetScan") };
			if (!datasetScanNode) {
				continue;
			}

			auto* locationItem{ new QTableWidgetItem(datasetScanNode.attribute("location").as_string()) };
			m_ui->tableWidget->setItem(rowIdx, 2, locationItem);
		}
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::checkCatalogsPath() {
	const QDir dir{m_prefs->THREDDSCatalogLocation+"/catalogs/"};
	if (!dir.exists()) {
#ifdef QT_DEBUG
	qDebug() << "Creating catalogs folder at: " << m_prefs->THREDDSCatalogLocation << "/catalogs/";
#endif
		if (!dir.mkpath(".")) {
			QMessageBox::critical(this,
								  tr("Permissions error..."),
								  tr("Failed to create the catalogs folder for THREDDS. This is probably a permissions issue. Ensure you have write permissions to: ")+
								  m_prefs->THREDDSCatalogLocation);
		}
	}
}

/***********************************************************************************/
bool WidgetThreddsConfig::validateDatasetName(const QString& datasetName) {
	// https://regex101.com/r/2MfejI/3
	const QRegularExpression re{"[\\.,;'`+*\\s/]+", QRegularExpression::CaseInsensitiveOption};

	const auto matchResult{ re.match(datasetName) };

	return !matchResult.hasMatch();
}

/***********************************************************************************/
void WidgetThreddsConfig::createRow(const QString& datasetName) {

	m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());
	const auto rowIdx{ m_ui->tableWidget->rowCount() - 1 };

	auto* nameItem{ new QTableWidgetItem(datasetName) };

	const auto catalogPath{ "catalogs/"+datasetName+".xml" };
	auto* pathItem{ new QTableWidgetItem(catalogPath) };

	m_ui->tableWidget->setItem(rowIdx, 0, nameItem);
	m_ui->tableWidget->setItem(rowIdx, 1, pathItem);
}
