#ifndef IOUTILS_H
#define IOUTILS_H

#include "datadownloaddesc.h"

#include <QString>

namespace IO {

// Location of Tomcat bin/ folder
const constexpr auto TOMCAT_BIN_DIR{ "/opt/tomcat9/bin" };
// Root location of where THREDDS datasets are stored
const constexpr auto THREDDS_DATA_DIR{ "" };

// Computes the disk location for a given dataset so THREDDS
// can read it
QString FindPathForDataset(const QString& filename);

QString FindPathForDataset(const DataDownloadDesc& data);

} // namespace IO

#endif // IOUTILS_H
