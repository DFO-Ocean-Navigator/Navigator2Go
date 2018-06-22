#include "ioutils.h"

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

} // namespace IO
