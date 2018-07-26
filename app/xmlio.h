#ifndef XMLIO_H
#define XMLIO_H

#include "nodiscard.h"

#include <QString>

#include <pugixml/pugixml.hpp>

#include <optional>

namespace IO {

/***********************************************************************************/
struct NODISCARD DatasetScanDesc {
	const QString DatasetName;
	const QString Location;
};

/***********************************************************************************/
// Reads an xml file from path. Returns std::nullopt on failure
NODISCARD std::optional<pugi::xml_document> readXML(const QString& path);

/***********************************************************************************/
// Appends a catalogRef tag to a catalog file.
// Returns the xlink:href to the dataset catalog to be created.
void appendDatasetToCatalog(pugi::xml_document& doc, const QString& datasetName);

/***********************************************************************************/
//
void addDataset(const QString& threddsCatalogLoc, const QString& datasetName, const QString& dataPath);

/***********************************************************************************/
// Checks if a named dataset exists in catalog.xml
NODISCARD bool datasetExists(const QString& threddsCatalogLoc, const QString& datasetName);

/***********************************************************************************/
// Creates a new catalog document with required headers/attributes
void createNewCatalogFile(const QString& threddsContentPath, const DatasetScanDesc& dataset);

/***********************************************************************************/
// Creates a new empty aggregate file with required tags.
NODISCARD pugi::xml_document createNewAggregateFile();

/***********************************************************************************/
// Returns a list of datasets from an opened catalog file.
NODISCARD QStringList getTHREDDSDatasetList(const pugi::xml_document& doc);

} // namespace IO

#endif // XMLIO_H
