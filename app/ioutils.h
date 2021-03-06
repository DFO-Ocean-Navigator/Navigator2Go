#ifndef IOUTILS_H
#define IOUTILS_H

#include "datadownloaddesc.h"
#include "netcdfimportdesc.h"

#include <QObject>
#include <QRunnable>
#include <QVector>

namespace IO {

/***********************************************************************************/
/// Location of Tomcat bin/ folder
#ifdef __linux__
const constexpr auto TOMCAT_BIN_DIR{"/opt/tomcat9/bin"};
#else
#endif
/// Location of Navigator frontend folder
#ifdef __linux__
const constexpr auto NAVIGATOR_FRONTEND_DIR{"/opt/Ocean-Data-Map-Project/"};
#else
#endif

/***********************************************************************************/
/// Wraps a netCDF file description for THREDDS.
struct NODISCARD THREDDSFileDesc {
  QString Path;
  const QString Filename;
};

/***********************************************************************************/
// Figures out the correct filename + path for a dataset
NODISCARD THREDDSFileDesc GetNCFilename(const QString &threddsContentDir,
                                        const DataDownloadDesc &data);

/***********************************************************************************/
/// Checks if a file exists at a given path
NODISCARD bool FileExists(const QString &path);

/***********************************************************************************/
void ClearPythonCache();

/***********************************************************************************/
/// Create a directory with necessary parents if needed.
void CreateDir(const QString &path);

/***********************************************************************************/
/// Recursively remove a directory
void RemoveDir(const QString &path);

/***********************************************************************************/
/// Finds the time dimension from a given NetCDF file path.
NODISCARD QString FindTimeDimension(const QString &netcdfFilePath);

/***********************************************************************************/
/// Copies (or moves) netCDF files to the THREDDS directory
/** This is always invoked by QThreadPool */
class CopyFilesRunnable : public QObject, public QRunnable {
  Q_OBJECT

public:
  CopyFilesRunnable(const QString threddsContentDir,
                    const bool removeSourceFiles,
                    QVector<NetCDFImportDesc> &&fileList);

  void run() override;

signals:
  void progress(const qint64 percentDone);
  void finished(const QStringList failedFiles);

private:
  const QString m_contentDir;
  const bool m_removeSourceNCFiles;
  const QVector<NetCDFImportDesc> m_fileList;
};

} // namespace IO

#endif // IOUTILS_H
