#include "filecopyworker.h"

namespace IO {

/***********************************************************************************/
FileCopyWorker::FileCopyWorker(QObject* parent) : QObject{parent} {

}

/***********************************************************************************/
void FileCopyWorker::copyFiles() {

	emit copyComplete();
}

} // namespace IO
