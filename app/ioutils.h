#ifndef IOUTILS_H
#define IOUTILS_H

#include "netcdfimportdesc.h"
#include "datadownloaddesc.h"

#include <QObject>
#include <QVector>
#include <QRunnable>

namespace IO {

/***********************************************************************************/
// Location of Tomcat bin/ folder
const constexpr auto TOMCAT_BIN_DIR{ "/opt/tomcat9/bin" };

/***********************************************************************************/
struct NODISCARD THREDDSFileDesc {
	QString Path;
	const QString Filename;
};

/***********************************************************************************/
// Figures out the correct filename + path for a dataset
NODISCARD THREDDSFileDesc GetTHREDDSFilename(const QString& threddsContentDir, const QString& sourceFilePath);
NODISCARD THREDDSFileDesc GetTHREDDSFilename(const QString& threddsContentDir, const DataDownloadDesc& data);

/***********************************************************************************/
// Checks if a file exists at a given path
NODISCARD bool FileExists(const QString& path);

/***********************************************************************************/
// Create a directory with necessary parents if needed.
void CreateDir(const QString& path);

/***********************************************************************************/
// Recursively remove a directory
void RemoveDir(const QString& path);

/***********************************************************************************/
// Finds the time dimension from a given NetCDF file path.
NODISCARD QString FindTimeDimension(const QString& netcdfFilePath);

/***********************************************************************************/
class CopyFilesRunnable : public QObject, public QRunnable {
	Q_OBJECT

public:
	CopyFilesRunnable(const QString threddsContentDir, QVector<NetCDFImportDesc>&& fileList);

	void run() override;

signals:
	void progress(const qint64 percentDone);
	void finished(const QStringList failedFiles);

private:
	const QString m_contentDir;
	const QVector<NetCDFImportDesc> m_fileList;
};

} // namespace IO

#endif // IOUTILS_H
