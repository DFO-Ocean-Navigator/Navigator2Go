#include "ioutils.h"

#include "datadownloaddesc.h"
#include "xmlio.h"

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>

#ifdef QT_DEBUG
#include <QDebug>
#endif

#include <netcdf4/ncFile.h>

namespace IO {

/***********************************************************************************/
THREDDSFileDesc GetNCFilename(const QString &threddsContentDir,
                              const DataDownloadDesc &data) {

  // Create filename
  const auto &fileName{data.ID + QStringLiteral("_") +
                       data.StartDate.toString(Qt::DateFormat::ISODate) +
                       (data.StartDate != data.EndDate
                            ? QStringLiteral("-") +
                                  data.EndDate.toString(Qt::DateFormat::ISODate)
                            : "") +
                       QStringLiteral(".nc")};

  // Find dataset location from thredds catalogs
  const auto &doc{readXML(threddsContentDir + QStringLiteral("/catalog.xml"))};
  const auto &ds{doc->child("catalog").find_child_by_attribute(
      "catalogRef", "xlink:title", data.ID.toStdString().c_str())};
  if (ds) {
    const auto &ds_catalog{IO::readXML(threddsContentDir + "/" +
                                       ds.attribute("xlink:href").as_string())};
    const auto &path{ds_catalog->child("catalog")
                         .child("datasetScan")
                         .attribute("location")
                         .as_string()};

    return {QString(path), QStringLiteral("/") + fileName};
  }

  // Dataset not found
  return {QStringLiteral(""), QStringLiteral("/") + fileName};
}

/***********************************************************************************/
bool FileExists(const QString &path) {
  const QFileInfo f{path};

  return f.exists() && f.isFile();
}

/***********************************************************************************/
void ClearPythonCache() {
  QDir d{QStringLiteral("/tmp/oceannavigator")};
  if (!d.exists()) {
    return;
  }

  QString status;
  d.removeRecursively() ? status = QStringLiteral("successful")
                        : status = QStringLiteral("failed");

  QMessageBox::information(nullptr, QObject::tr("Clear python cache"),
                           QObject::tr("Cache clearing operation ") + status +
                               ".");

  return;
}

/***********************************************************************************/
void CreateDir(const QString &path) {
  const QFileInfo f{path};
  const QDir dir{f.absoluteDir()};

  dir.mkpath(QStringLiteral("."));
}

/***********************************************************************************/
void RemoveDir(const QString &path) {
  QDir dir{path};
  dir.removeRecursively();
}

/***********************************************************************************/
QString FindTimeDimension(const QString &netcdfFilePath) {
  using namespace netCDF;
  NcFile f(netcdfFilePath.toStdString(), NcFile::FileMode::read);

  const QRegularExpression rxp{QStringLiteral("(time)"),
                               QRegularExpression::CaseInsensitiveOption};

  const auto &variableList{f.getCoordVars()};
  for (const auto &variable : variableList) {
    if (const auto &result{rxp.match(variable.first.c_str())};
        result.hasMatch()) {
      return variable.first.c_str();
    }
  }

  return "";
}

/***********************************************************************************/
CopyFilesRunnable::CopyFilesRunnable(const QString threddsContentDir,
                                     const bool removeSourceFiles,
                                     QVector<NetCDFImportDesc> &&fileList)
    : m_contentDir{threddsContentDir}, m_removeSourceNCFiles{removeSourceFiles},
      m_fileList{std::move(fileList)} {}

/***********************************************************************************/
void CopyFilesRunnable::run() {
  const auto stepSize{100 * (1 / static_cast<qint64>(m_fileList.size()))};
  qint64 currentStep{0};

  const auto &catalog{IO::readXML(m_contentDir + QStringLiteral("/catalog.xml"))
                          ->child("catalog")};
  QStringList errorList;

  for (const auto &file : m_fileList) {
    const auto &fileName{QFileInfo{file.File}.fileName()};
    const auto &href{QStringLiteral("catalogs/") + file.DatasetToAppendTo +
                     QStringLiteral(".xml")};
    const auto *dsCatalogPath{
        catalog
            .find_child_by_attribute("catalogRef", "xlink:href",
                                     href.toStdString().c_str())
            .attribute("xlink:href")
            .as_string()};
    const auto *dsLocation{
        IO::readXML(m_contentDir + QStringLiteral("/") + dsCatalogPath)
            ->child("catalog")
            .child("datasetScan")
            .attribute("location")
            .as_string()};

    if (!QFile::copy(file.File,
                     QString(dsLocation) + QStringLiteral("/") + fileName)) {
      errorList.append(file.File);
    } else {
      if (m_removeSourceNCFiles) {
        QFile::remove(file.File);
      }
    }

    currentStep += stepSize;
    emit progress(currentStep);
  }

  emit finished(errorList);
}

} // namespace IO
