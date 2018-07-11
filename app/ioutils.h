#ifndef IOUTILS_H
#define IOUTILS_H

#include "datadownloaddesc.h"

#include <QObject>
#include <QRunnable>
#include <QString>

namespace IO {

/***********************************************************************************/
// Location of Tomcat bin/ folder
const constexpr auto TOMCAT_BIN_DIR{ "/opt/tomcat9/bin" };

/***********************************************************************************/
// Computes the disk location for a given dataset so THREDDS
// can read it
QString FindPathForDataset(const QString& filename);

QString FindPathForDataset(const DataDownloadDesc& data);

/***********************************************************************************/
NODISCARD bool FileExists(const QString& path);

/***********************************************************************************/
// Create a directory with necessary parents if needed.
void CreateDir(const QString& path);

/***********************************************************************************/
// Recursively remove a directory
void RemoveDir(const QString& path);

/***********************************************************************************/
NODISCARD QString FindTimeDimension(const QString& netcdfFilePath);

/***********************************************************************************/
class CopyFilesRunnable : public QObject, public QRunnable {
	Q_OBJECT

public:
	explicit CopyFilesRunnable(QStringList&& fileList);

	void run() override;

signals:
	void progress(const qint64 percentDone);
	void finished(const QStringList failedFiles);

private:
	const QStringList m_fileList;
};

} // namespace IO

#endif // IOUTILS_H
