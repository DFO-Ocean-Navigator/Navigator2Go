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

	bool ok_dsname;
	const auto& datasetName{ QInputDialog::getText(	this,
													tr("Add THREDDS Dataset..."),
													tr("Enter a dataset name.\nNo whitespace, /, \\, +, *, ., characters allowed."),
													QLineEdit::Normal,
													"my_dataset_" + QString::number(qrand()),
													&ok_dsname)
						   };

	bool ok_dspath;
	const auto& datasetPath{ QInputDialog::getText( this,
												   tr("Dataset Path..."),
												   tr("Enter the path for the folder containing the dataset."),
												   QLineEdit::Normal,
												   "",
												   &ok_dspath)
						  };

	if (ok_dsname && validateDatasetName(datasetName) && ok_dspath) {
		createRow(datasetName, datasetPath);
		addDataset(datasetName, datasetPath);

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
void WidgetThreddsConfig::buildTable() {

	const auto& catalogFile{ m_prefs->THREDDSCatalogLocation + QString("/catalog.xml") };

	const auto doc{ IO::readXML(catalogFile) };

	if (!doc.has_value()) {
		QMessageBox box{this};
		box.setWindowTitle(tr("THREDDS catalog error..."));
		box.setText(tr("Failed to load THREDDS catalog file: ") + catalogFile);
		box.setIcon(QMessageBox::Critical);

		box.exec();

		return;
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
void WidgetThreddsConfig::addDataset(const QString& datasetName, const QString& dataPath) {

	const auto& path{ QString("catalogs/") + datasetName };

	// Modify catalog.xml
	{
		const auto& catalogPath{ m_prefs->THREDDSCatalogLocation + QString("/catalog.xml") };
		const auto doc{ IO::readXML(catalogPath) };
		auto child{ doc->child("catalog").append_child("catalogRef") };
		child.append_attribute("xlink:title") = datasetName.toStdString().c_str();
		child.append_attribute("xlink:href") = std::strcat(path.toLatin1().data(),".xml");
		child.append_attribute("name") = "";

		doc->save_file(catalogPath.toStdString().c_str());
	}

	const auto& fileName{ m_prefs->THREDDSCatalogLocation + "/" + path + ".xml"};
	if (!IO::FileExists(fileName)) {
		IO::CreateDir(fileName);
	}

	// Create dataset catalog file (giops_day.xml for example)
	auto catalog{ IO::readXML(path) };
	if (!catalog.has_value()) {
		catalog = IO::createNewCatalogFile();

		auto datasetScan{ catalog->child("catalog").append_child() };
		datasetScan.set_name("datasetScan");
		datasetScan.append_attribute("name") = datasetName.toStdString().c_str();
		datasetScan.append_attribute("ID") = datasetName.toLower().toStdString().c_str();
		datasetScan.append_attribute("path") = datasetName.toLower().toStdString().c_str();
		datasetScan.append_attribute("location") = dataPath.toStdString().c_str();

		auto serviceName{ datasetScan.append_child() };
		serviceName.set_name("serviceName");
		serviceName.text().set("all");

		catalog->save_file(fileName.toStdString().c_str());
	}

	// Create dataset aggregate file
	const auto aggregatePath{ dataPath + "/aggregated.ncml "};
	auto aggregate{ IO::readXML(aggregatePath) };
	if (!aggregate.has_value()) {
		// Find netCDF files in target directory
		QDir dir{dataPath};
		if (!dir.exists()) {
			dir.mkpath(".");
		}
		dir.setNameFilters({"*.nc"});

		QString timeDimension;
		if (dir.entryInfoList().empty()) {
			timeDimension = QInputDialog::getText(this,
												  tr("Enter time dimension"),
												  tr("The folder you specified is empty. Please enter the name of the time dimension for your dataset. Alternatively, add a netCDF file to this directory and re-run this wizard."));

			if (timeDimension.isEmpty()) {
				return;
			}
		}
		else {
			// Find the name of time dimension of first file.
			timeDimension = IO::FindTimeDimension(dir.entryInfoList()[0].absoluteFilePath());
		}

		aggregate = IO::createNewAggregateFile();

		auto aggregation{ aggregate->child("netcdf").append_child() };
		aggregation.set_name("aggregation");
		aggregation.append_attribute("type") = "joinExisting";
		aggregation.append_attribute("recheckEvery") = "1 hour";

		aggregation.append_attribute("dimName") = timeDimension.toStdString().c_str();
		auto scan{ aggregation.append_child() };
		scan.set_name("scan");
		scan.append_attribute("location") = dataPath.toStdString().c_str();
		scan.append_attribute("suffix") = ".nc";
		scan.append_attribute("recheckEvery") = "1 hour";

		aggregate->save_file(aggregatePath.toStdString().c_str());
	}
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


