#ifndef XMLIO_H
#define XMLIO_H

#include "nodiscard.h"

#include <QString>

#include <pugixml/pugixml.hpp>

#include <optional>

namespace IO {

/***********************************************************************************/
// Reads an xml file from path. Returns std::nullopt on failure
NODISCARD std::optional<pugi::xml_document> readXML(const QString& path);

/***********************************************************************************/
// Creates a new catalog document with required headers/attributes
NODISCARD pugi::xml_document createNewCatalogFile();

/***********************************************************************************/
// Creates a new empty aggregate file with required tags.
NODISCARD pugi::xml_document createNewAggregateFile();

/***********************************************************************************/
// Returns a list of datasets from an opened catalog file.
NODISCARD QStringList getTHREDDSDatasetList(const pugi::xml_document& doc);

} // namespace IO

#endif // XMLIO_H
