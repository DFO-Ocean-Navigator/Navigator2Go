#include "ioutils.h"

#include <QFileInfo>

namespace IO {

/***********************************************************************************/
QString FindPathForDataset(const QString& filename) {
	return {};
}

/***********************************************************************************/
QString FindPathForDataset(const DataDownloadDesc& data) {
	return	data.ID + "_" +
			data.SelectedVariables.join(",") + "_" +
			data.StartDate.toString(Qt::DateFormat::ISODate) +
			(data.StartDate != data.EndDate ? "_" + data.EndDate.toString(Qt::DateFormat::ISODate) : "");
}

/***********************************************************************************/
bool FileExists(const QString& path) {
	const QFileInfo f{path};

	if (f.exists() && f.isFile()) {
		return true;
	}

	return false;
}

/***********************************************************************************/
CopyFilesRunnable::CopyFilesRunnable(QStringList&& fileList) : m_fileList{std::move(fileList)} {
}

/***********************************************************************************/
void CopyFilesRunnable::run() {
	const auto stepSize{ 100 * (1 / static_cast<qint64>(m_fileList.size())) };
	qint64 currentStep{ 0 };

	QStringList errorList;

	for (const auto& file : m_fileList) {

		const auto fileName{ QFileInfo{file}.fileName() };

		if (!QFile::rename(file, FindPathForDataset(fileName))) {
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
