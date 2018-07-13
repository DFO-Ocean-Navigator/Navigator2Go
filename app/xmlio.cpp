#include "xmlio.h"

namespace IO {

/***********************************************************************************/
std::optional<pugi::xml_document> readXML(const QString& path) {
	pugi::xml_document doc;

	const auto& result{ doc.load_file(path.toStdString().c_str()) };

	return result ? std::make_optional(std::move(doc)) : std::nullopt;
}

/***********************************************************************************/
pugi::xml_document createNewCatalogFile() {
	pugi::xml_document doc;

	// Header declaration
	auto header{ doc.prepend_child(pugi::node_declaration) };
	header.append_attribute("version") = 1.0;
	header.append_attribute("encoding") = "UTF-8";

	// Top-level <catalog></catalog> tags
	auto catalog{ doc.append_child() };
	catalog.set_name("catalog");
	catalog.append_attribute("xmlns") = "http://www.unidata.ucar.edu/namespaces/thredds/InvCatalog/v1.0";
	catalog.append_attribute("xmlns:xlink") = "http://www.w3.org/1999/xlink";
	catalog.append_attribute("name") = "Unidata THREDDS-IDD NetCDF-OpenDAP Server";
	catalog.append_attribute("version") = "1.0.1";

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

	return doc;
}

/***********************************************************************************/
pugi::xml_document createNewAggregateFile() {
	pugi::xml_document doc;

	auto netCDFRoot{ doc.append_child() };
	netCDFRoot.set_name("netcdf");
	netCDFRoot.append_attribute("xmlns") = "http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2";

	return doc;
}

} // namespace IO
