#include "xmlio.h"

#include "ioutils.h"

#include <QStringList>
#include <QVector>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>

#include <cstring>

#ifdef QT_DEBUG
	#include <QDebug>
#endif

namespace IO {

/***********************************************************************************/
std::optional<pugi::xml_document> readXML(const QString& path) {
	pugi::xml_document doc;

	const auto& result{ doc.load_file(path.toStdString().c_str()) };

	return result ? std::make_optional(std::move(doc)) : std::nullopt;
}

/***********************************************************************************/
void appendDatasetToCatalog(pugi::xml_document& doc, const QString& datasetName) {
	auto child{ doc.child("catalog").append_child() };

	child.append_attribute("xlink:title") = datasetName.toStdString().c_str();
	const auto& href{ "catalogs/" + datasetName };
	child.append_attribute("xlink:href") = href.toStdString().c_str();
}

/***********************************************************************************/
bool addDataset(const QString& rootCatalogFolder, const QString& datasetName, const QString& dataPath) {
	const auto& path{ QStringLiteral("catalogs/") + datasetName };

	// Modify catalog.xml
	const auto catalogPath{ rootCatalogFolder + QString("/catalog.xml") };
	const auto doc{ IO::readXML(catalogPath) };
	auto child{ doc->child("catalog").append_child("catalogRef") };
	child.append_attribute("xlink:title") = datasetName.toStdString().c_str();
	child.append_attribute("xlink:href") = QString(path + ".xml").toStdString().c_str();
	child.append_attribute("name") = "";

	const auto& fileName{ rootCatalogFolder + "/" + path + ".xml"};
	if (!FileExists(fileName)) {
		CreateDir(fileName);
	}

	// Create dataset catalog file (giops_day.xml for example)
	if (auto catalog{ IO::readXML(path) }; !catalog.has_value()) {
		createNewCatalogFile(rootCatalogFolder, { datasetName, dataPath } );
	}

	// Create dataset aggregate file
	const auto aggregatePath{ dataPath + QStringLiteral("/aggregated.ncml") };
	auto aggregate{ IO::readXML(aggregatePath) };
	if (!aggregate.has_value()) {
		// Find netCDF files in target directory
		QDir dir{dataPath};
		if (!dir.exists()) {
			dir.mkpath(QStringLiteral("."));
		}
		dir.setNameFilters({QStringLiteral("*.nc")});

		QString timeDimension;
		if (dir.entryInfoList().empty()) {
			timeDimension = QInputDialog::getText(nullptr,
												  QObject::tr("Enter time dimension"),
												  QObject::tr("The folder you specified is empty. Please enter the name of the time dimension for your dataset. Alternatively, add a netCDF file to this directory and re-run this wizard."));

			if (timeDimension.isEmpty()) {
				return false;
			}
		}
		else {
			// Find the name of time dimension of first file.
			timeDimension = FindTimeDimension(dir.entryInfoList()[0].absoluteFilePath());
		}
		doc->save_file(catalogPath.toStdString().c_str());

		aggregate = createNewAggregateFile();

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

	/* Show aggregation URL dialog...not really needed anymore since it's auto-filled in the dataset
	 * properties
	QMessageBox::information(nullptr,
							 QObject::tr("Dataset Aggregation URL"),
							 QString("localhost:8080/thredds/dodsC/%1/aggregated.ncml").arg(datasetName));
	*/

	return true;
}

/***********************************************************************************/
void removeDataset(const QString& rootCatalogFolder, const QString& datasetName, const QString& dataPath) {

	// Modify catalog.xml
	{
		const auto& catalogPath{ rootCatalogFolder + QStringLiteral("/catalog.xml") };
		auto doc{ IO::readXML(catalogPath) };
		const auto& nodeToRemove{ doc->child("catalog").find_child_by_attribute("catalogRef", "xlink:title", datasetName.toStdString().c_str()) };
		nodeToRemove.parent().remove_child(nodeToRemove);

		doc->save_file(catalogPath.toStdString().c_str());
	}

	// Remove dataset catalog
	QFile::remove(rootCatalogFolder + QStringLiteral("/catalogs/") + datasetName + QStringLiteral(".xml"));

	// Remove datasets + aggregated file
	RemoveDir(dataPath);
}

/***********************************************************************************/
bool datasetExists(const QString& threddsCatalogLoc, const QString& datasetName) {
	const auto& doc{ readXML(threddsCatalogLoc + QStringLiteral("/catalog.xml")) };

	return !doc->child("catalog").find_child_by_attribute("catalogRef", "xlink:title", datasetName.toStdString().c_str()).empty();
}

/***********************************************************************************/
void createNewCatalogFile(const QString& threddsContentPath, const DatasetScanDesc& dataset) {
	pugi::xml_document doc;

	// Header declaration
	auto header{ doc.prepend_child(pugi::node_declaration) };
	header.append_attribute("version") = "1.0";
	header.append_attribute("encoding") = "UTF-8";

	// Top-level <catalog></catalog> tags
	auto catalog{ doc.append_child() };
	catalog.set_name("catalog");
	catalog.append_attribute("xmlns") = "http://www.unidata.ucar.edu/namespaces/thredds/InvCatalog/v1.0";
	catalog.append_attribute("xmlns:xlink") = "http://www.w3.org/1999/xlink";
	catalog.append_attribute("name") = "Navigator2Go THREDDS Server";
	catalog.append_attribute("version") = "1.0";

	{
		// Service tags
		auto services{ catalog.append_child() };
		services.set_name("service");
		services.append_attribute("name") = "all";
		services.append_attribute("base") = "";
		services.append_attribute("serviceType") = "compound";


		auto odapService{ services.append_child( ) };
		odapService.set_name("service");
		odapService.append_attribute("name") = "odap";
		odapService.append_attribute("serviceType") = "OpenDAP";
		odapService.append_attribute("base") = "/thredds/dodsC/";

		auto dap4Service{ services.append_child( ) };
		dap4Service.set_name("service");
		dap4Service.append_attribute("name") = "dap4";
		dap4Service.append_attribute("serviceType") = "DAP4";
		dap4Service.append_attribute("base") = "/thredds/dap4/";

		auto httpService{ services.append_child() };
		httpService.set_name("service");
		httpService.append_attribute("name") = "http";
		httpService.append_attribute("serviceType") = "HTTPServer";
		httpService.append_attribute("base") = "/thredds/fileServer/";

		auto ncssService{ services.append_child() };
		ncssService.set_name("service");
		ncssService.append_attribute("name") = "ncss";
		ncssService.append_attribute("serviceType") = "NetcdfSubset";
		ncssService.append_attribute("base") = "/thredds/ncss/";
	}

	{
		auto service{ catalog.append_child() };
		service.set_name("service");
		service.append_attribute("name") = "dap";
		service.append_attribute("serviceType") = "compound";
		service.append_attribute("base") = "";

		auto odapService{ service.append_child() };
		odapService.set_name("service");
		odapService.append_attribute("name") = "odap";
		odapService.append_attribute("serviceType") = "OpenDAP";
		odapService.append_attribute("base") = "/thredds/dodsC/";

		auto dap4Service{ service.append_child() };
		dap4Service.set_name("service");
		dap4Service.append_attribute("name") = "dap4";
		dap4Service.append_attribute("serviceType") = "DAP4";
		dap4Service.append_attribute("base") = "/thredds/dap4/";
	}

	auto datasetScan{ catalog.append_child() };
	datasetScan.set_name("datasetScan");
	const auto* nameChar{ dataset.DatasetName.toLower().toStdString().c_str() };

	datasetScan.append_attribute("name") = nameChar;
	datasetScan.append_attribute("ID") = nameChar;
	datasetScan.append_attribute("path") = nameChar;
	datasetScan.append_attribute("location") = dataset.Location.toStdString().c_str();

	auto serviceName{ datasetScan.append_child() };
	serviceName.set_name("serviceName");
	serviceName.text().set("all");

	const auto& filename{ threddsContentPath + QStringLiteral("/catalogs/") + dataset.DatasetName.toLower() + ".xml" };
	doc.save_file(filename.toStdString().c_str());
}

/***********************************************************************************/
pugi::xml_document createNewAggregateFile() {
	pugi::xml_document doc;

	auto netCDFRoot{ doc.append_child() };
	netCDFRoot.set_name("netcdf");
	netCDFRoot.append_attribute("xmlns") = "http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2";

	return doc;
}

/***********************************************************************************/
QStringList getTHREDDSDatasetList(const pugi::xml_document& doc) {
	QStringList dsList;

	for (const auto& child : doc.child("catalog")) {
		if (std::strcmp(child.name(), "catalogRef") == 0) {
			dsList << child.attribute("xlink:title").as_string();
		}
	}

	return dsList;
}

/***********************************************************************************/
void createNewPrimaryCatalog(const QString& threddsContentPath) {
	pugi::xml_document doc;

	// Header declaration
	auto header{ doc.prepend_child(pugi::node_declaration) };
	header.append_attribute("version") = "1.0";
	header.append_attribute("encoding") = "UTF-8";

	// Top-level <catalog></catalog> tags
	auto catalog{ doc.append_child() };
	catalog.set_name("catalog");
	catalog.append_attribute("xmlns") = "http://www.unidata.ucar.edu/namespaces/thredds/InvCatalog/v1.0";
	catalog.append_attribute("xmlns:xlink") = "http://www.w3.org/1999/xlink";
	catalog.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
	catalog.append_attribute("xsi:schemaLocation") = "http://www.unidata.ucar.edu/namespaces/thredds/InvCatalog/v1.0 http://www.unidata.ucar.edu/schemas/thredds/InvCatalog.1.0.6.xsd";
	catalog.append_attribute("name") = "Navigator2Go THREDDS Server";

	{
		// Service tags
		auto services{ catalog.append_child() };
		services.set_name("service");
		services.append_attribute("name") = "all";
		services.append_attribute("base") = "";
		services.append_attribute("serviceType") = "compound";


		auto odapService{ services.append_child( ) };
		odapService.set_name("service");
		odapService.append_attribute("name") = "odap";
		odapService.append_attribute("serviceType") = "OpenDAP";
		odapService.append_attribute("base") = "/thredds/dodsC/";

		auto dap4Service{ services.append_child( ) };
		dap4Service.set_name("service");
		dap4Service.append_attribute("name") = "dap4";
		dap4Service.append_attribute("serviceType") = "DAP4";
		dap4Service.append_attribute("base") = "/thredds/dap4/";

		auto httpService{ services.append_child() };
		httpService.set_name("service");
		httpService.append_attribute("name") = "http";
		httpService.append_attribute("serviceType") = "HTTPServer";
		httpService.append_attribute("base") = "/thredds/fileServer/";

		auto ncssService{ services.append_child() };
		ncssService.set_name("service");
		ncssService.append_attribute("name") = "ncss";
		ncssService.append_attribute("serviceType") = "NetcdfSubset";
		ncssService.append_attribute("base") = "/thredds/ncss/";
	}

	{
		auto service{ catalog.append_child() };
		service.set_name("service");
		service.append_attribute("name") = "dap";
		service.append_attribute("serviceType") = "compound";
		service.append_attribute("base") = "";

		auto odapService{ service.append_child() };
		odapService.set_name("service");
		odapService.append_attribute("name") = "odap";
		odapService.append_attribute("serviceType") = "OpenDAP";
		odapService.append_attribute("base") = "/thredds/dodsC/";

		auto dap4Service{ service.append_child() };
		dap4Service.set_name("service");
		dap4Service.append_attribute("name") = "dap4";
		dap4Service.append_attribute("serviceType") = "DAP4";
		dap4Service.append_attribute("base") = "/thredds/dap4/";
	}

	// Check the path exists
	IO::CreateDir(threddsContentPath);

	const auto& filename{ threddsContentPath + QStringLiteral("/catalog.xml") };
	doc.save_file(filename.toStdString().c_str());
}

} // namespace IO
