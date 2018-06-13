#include "filecopyworker.h"

#include "ioutils.h"

#include <QFileInfo>

namespace IO {

/***********************************************************************************/
FileCopyWorker::FileCopyWorker(const QStringList& fileList, QObject* parent) :	QObject{parent},
																				m_fileList{fileList} {
}

/***********************************************************************************/
void FileCopyWorker::copyFiles() {

	const auto stepSize = 100 * (1 / static_cast<std::size_t>(m_fileList.size()));
	std::size_t currentStep = 0;

	for (const auto& filePath : m_fileList) {
		const auto fileName{ QFileInfo{filePath}.fileName() };

		if (!QFile::rename(filePath, IO::FindPathForDataset(filePath))) {
			emit error(tr("Failed to move: ") + filePath);
		}
		else {
			if (!QFile::remove(fileName)) {
				emit error(tr("Failed to delete: ") + filePath + ". This file was imported though!");
			}
		}

		currentStep += stepSize;
		emit progress(currentStep);
	}

	emit finished();
}

} // namespace IO
