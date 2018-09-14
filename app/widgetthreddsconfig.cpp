#include "widgetthreddsconfig.h"
#include "ui_widgetthreddsconfig.h"

#include "dialogthreddslogs.h"
#include "xmlio.h"
#include "preferences.h"
#include "constants.h"
#include "ioutils.h"

#include <QFile>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>
#include <QInputDialog>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>

#include <pugixml/pugixml.hpp>

#include <cstring>

#ifdef QT_DEBUG
	#include <QDebug>
#endif


/***********************************************************************************/
WidgetThreddsConfig::WidgetThreddsConfig(QWidget* parent, const Preferences* prefs) :	QWidget{parent},
																						m_ui{new Ui::WidgetThreddsConfig},
																						m_prefs{prefs} {
	m_ui->setupUi(this);

	m_ui->labelTHREDDSPath->setText(tr("THREDDS Location: ") + m_prefs->THREDDSCatalogLocation);
	m_ui->labelTHREDDSPath->setStyleSheet(QSS_COLOR_GREEN);

	checkCatalogsPath();

	BuildTable();
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

	bool ok_dsname;
	const auto& datasetName{ QInputDialog::getText(	this,
													tr("Add THREDDS Dataset..."),
													tr("Enter a dataset name.\nNo whitespace, /, \\, +, *, ., characters allowed."),
													QLineEdit::Normal,
													"my_dataset_" + QString::number(qrand()),
													&ok_dsname)
						   };

	if (!ok_dsname) {
		return;
	}
	bool ok_dspath;
	const auto& datasetPath{ QInputDialog::getText( this,
												   tr("Dataset Path..."),
												   tr("Enter the path for the folder containing the dataset."),
												   QLineEdit::Normal,
												   "/opt/thredds_content/data/"+datasetName,
												   &ok_dspath)
						  };

	if (validateDatasetName(datasetName) && ok_dspath) {

		if (!IO::addDataset(m_prefs->THREDDSCatalogLocation, datasetName, datasetPath)) {
			return;
		}

		createRow(datasetName, datasetPath);
		QMessageBox::information(this,
								 tr("Success..."),
								 tr("Your dataset has been added to THREDDS! Go to the Dashboard to Stop and Start the Apache server to apply your changes."));
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::on_pushButtonRemoveDataset_clicked() {

	if (const auto currentRow{ m_ui->tableWidget->currentRow() }; currentRow != -1) {

		const auto reply{ QMessageBox::warning(this,
											   tr("Confirm Action"),
											   tr("Delete selected dataset? Warning: this will remove the netCDF files too!"),
											   QMessageBox::Yes | QMessageBox::No)
						};

		if (reply == QMessageBox::Yes) {
			removeDataset(m_ui->tableWidget->item(currentRow, 0)->text(),
						  m_ui->tableWidget->item(currentRow, 2)->text()
						  );

			m_ui->tableWidget->removeRow(m_ui->tableWidget->currentRow());
		}
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::on_pushButtonShowLogs_clicked() {
	DialogTHREDDSLogs dlg{ m_prefs->THREDDSCatalogLocation + "/logs", this};

	dlg.exec();
}

/***********************************************************************************/
void WidgetThreddsConfig::BuildTable() {
	m_ui->tableWidget->clear();
	m_ui->tableWidget->horizontalHeader()->setStretchLastSection(true); // Resize columns to widget width
	m_ui->tableWidget->setHorizontalHeaderLabels({
													tr("Dataset Name"),
													tr("Catalog Path"),
													tr("Dataset Files Location")
												 });

	const auto& catalogFile{ m_prefs->THREDDSCatalogLocation + QString("/catalog.xml") };

	auto doc{ IO::readXML(catalogFile) };

	if (!doc.has_value()) {
		IO::createNewPrimaryCatalog(m_prefs->THREDDSCatalogLocation);

		doc = IO::readXML(catalogFile);
	}

	const auto& rootNode{ doc->child("catalog") };

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

			auto* const nameItem{ new QTableWidgetItem(node.attribute("xlink:title").as_string()) };
			nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable); // Make read-only

			const auto& catalogPath{ node.attribute("xlink:href").as_string() };
			auto* const pathItem{ new QTableWidgetItem(catalogPath) };
			pathItem->setFlags(pathItem->flags() ^ Qt::ItemIsEditable); // Make read-only

			m_ui->tableWidget->setItem(rowIdx, 0, nameItem);
			m_ui->tableWidget->setItem(rowIdx, 1, pathItem);


			const auto& datasetCatalog{ IO::readXML(m_prefs->THREDDSCatalogLocation + "/" + catalogPath) };
			if (!datasetCatalog.has_value()) {
				continue;
			}

			const auto& datasetScanNode{ datasetCatalog->child("catalog").child("datasetScan") };
			if (!datasetScanNode) {
				continue;
			}

			auto* const locationItem{ new QTableWidgetItem(datasetScanNode.attribute("location").as_string()) };
			locationItem->setFlags(locationItem->flags() ^ Qt::ItemIsEditable); // Make read-only
			m_ui->tableWidget->setItem(rowIdx, 2, locationItem);
		}
	}
}

/***********************************************************************************/
void WidgetThreddsConfig::checkCatalogsPath() {
	if (const QDir dir{m_prefs->THREDDSCatalogLocation+"/catalogs/"}; !dir.exists()) {
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

	const auto& matchResult{ re.match(datasetName) };

	return !matchResult.hasMatch();
}

/***********************************************************************************/
void WidgetThreddsConfig::createRow(const QString& datasetName, const QString& dataPath) {

	m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());
	const auto rowIdx{ m_ui->tableWidget->rowCount() - 1 };

	auto* const nameItem{ new QTableWidgetItem(datasetName) };
	nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);

	const auto& catalogPath{ "catalogs/"+datasetName+".xml" };
	auto* const pathItem{ new QTableWidgetItem(catalogPath) };
	pathItem->setFlags(pathItem->flags() ^ Qt::ItemIsEditable);

	auto* const dataPathItem{ new QTableWidgetItem(dataPath) };
	dataPathItem->setFlags(dataPathItem->flags() ^ Qt::ItemIsEditable);

	m_ui->tableWidget->setItem(rowIdx, 0, nameItem);
	m_ui->tableWidget->setItem(rowIdx, 1, pathItem);
	m_ui->tableWidget->setItem(rowIdx, 2, dataPathItem);
}

/***********************************************************************************/
void WidgetThreddsConfig::removeDataset(const QString& datasetName, const QString& dataPath) {

	// Modify catalog.xml
	{
		const auto& catalogPath{ m_prefs->THREDDSCatalogLocation + QString("/catalog.xml") };
		auto doc{ IO::readXML(catalogPath) };
		const auto& nodeToRemove{ doc->child("catalog").find_child_by_attribute("catalogRef", "xlink:title", datasetName.toStdString().c_str()) };
		nodeToRemove.parent().remove_child(nodeToRemove);

		doc->save_file(catalogPath.toStdString().c_str());
	}

	// Remove dataset catalog
	QFile::remove(m_prefs->THREDDSCatalogLocation + "/catalogs/" + datasetName + ".xml");

	// Remove datasets + aggregated file
	IO::RemoveDir(dataPath);
}

/***********************************************************************************/
void WidgetThreddsConfig::on_pushButtonBrowseTHREDDSDir_clicked() {
	QDesktopServices::openUrl({"/opt/thredds_content/"});
}
