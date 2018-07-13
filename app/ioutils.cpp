#include "ioutils.h"

#include "datadownloaddesc.h"

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QDebug>

#include <netcdf4/ncFile.h>

namespace IO {

/***********************************************************************************/
QString FindPathForDataset(const QString& threddsContentDir, const QString& sourceFilePath) {

}

/***********************************************************************************/
QString FindPathForDataset(const QString& threddsContentDir, const DataDownloadDesc& data) {
	return	data.ID + "_" +
			data.SelectedVariables.join(",") + "_" +
			data.StartDate.toString(Qt::DateFormat::ISODate) +
			(data.StartDate != data.EndDate ? "_" + data.EndDate.toString(Qt::DateFormat::ISODate) : "");
}

/***********************************************************************************/
bool FileExists(const QString& path) {
	const QFileInfo f{path};

	return f.exists() && f.isFile();
}

/***********************************************************************************/
void CreateDir(const QString& path) {
	const QFileInfo f{path};
	const QDir dir{f.absoluteDir()};

	dir.mkpath(".");
}

/***********************************************************************************/
void RemoveDir(const QString& path) {
	QDir dir{path};
	dir.removeRecursively();
}

/***********************************************************************************/
QString FindTimeDimension(const QString& netcdfFilePath) {
	using namespace netCDF;
	NcFile f;

	try {
		f.open(netcdfFilePath.toStdString(), NcFile::FileMode::read);
	} catch (const netCDF::exceptions::NcException& e) {
		qDebug() << e.what();
	}

	const QRegularExpression rxp{"(time)", QRegularExpression::CaseInsensitiveOption};

	const auto& variableList{ f.getCoordVars() };
	for (const auto& variable : variableList) {
		const auto& result{ rxp.match(variable.first.c_str()) };

		if (result.hasMatch()) {
			return variable.first.c_str();
		}
	}

	return "";
}

/***********************************************************************************/
CopyFilesRunnable::CopyFilesRunnable(const QString threddsContentDir, QStringList&& fileList) : m_contentDir{threddsContentDir},
																								m_fileList{std::move(fileList)} {
}

/***********************************************************************************/
void CopyFilesRunnable::run() {
	const auto stepSize{ 100 * (1 / static_cast<qint64>(m_fileList.size())) };
	qint64 currentStep{ 0 };

	QStringList errorList;

	for (const auto& file : m_fileList) {

		const auto& fileName{ QFileInfo{file}.fileName() };

		if (!QFile::rename(file, FindPathForDataset(m_contentDir, fileName))) {
			errorList.append(file);
		}
		else {
			QFile::remove(file);
		}

		currentStep += stepSize;
		emit progress(currentStep);
	}

	emit finished(errorList);
}


} // namespace IO
