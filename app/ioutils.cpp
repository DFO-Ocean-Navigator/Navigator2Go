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
CopyFilesRunnable::CopyFilesRunnable(QStringList&& fileList) : m_fileList{std::move(fileList)} {

}

/***********************************************************************************/
void CopyFilesRunnable::run() {
	const auto stepSize{ 100 * (1 / static_cast<std::size_t>(m_fileList.size())) };
	std::size_t currentStep{ 0 };

	std::vector<QString> errorList, desFileLocs;

	std::size_t idx = 0;
	for (const auto& file : m_fileList) {

		if (!QFile::rename(file, desFileLocs[idx])) {
			errorList.emplace_back(file);
		}
		else {
			QFile::remove(file);
		}

		currentStep += stepSize;
		emit progress(currentStep);
		++idx;
	}

	emit finished(errorList);
}

} // namespace IO
