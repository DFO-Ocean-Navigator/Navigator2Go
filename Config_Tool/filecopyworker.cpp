#include "filecopyworker.h"

#include <QFileInfo>

namespace IO {

/***********************************************************************************/
FileCopyWorker::FileCopyWorker(const QStringList& fileList, QObject* parent) :	QObject{parent},
																				m_fileList{fileList} {
}

/***********************************************************************************/
void FileCopyWorker::copyFiles() {

	for (const auto& filePath : m_fileList) {
		const auto fileName{ QFileInfo{filePath}.fileName() };
		//QFile::rename(fileName, ".nc");

		//QFile::remove(fileName);
	}

	emit finished();
}

} // namespace IO
