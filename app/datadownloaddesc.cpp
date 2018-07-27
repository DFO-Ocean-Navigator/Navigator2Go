#include "datadownloaddesc.h"

#include <QUrlQuery>
/***********************************************************************************/
QString DataDownloadDesc::GetAPIQuery(const QString& remoteURL) const {
	QUrlQuery q{remoteURL + "/subset/?"};
	q.addQueryItem("variables", SelectedVariables.join(","));
	q.addQueryItem("quantum", Quantum);
	q.addQueryItem("dataset_name", ID);
	q.addQueryItem("time", StartDate.toString(Qt::DateFormat::ISODate) + "," + EndDate.toString(Qt::DateFormat::ISODate));
	q.addQueryItem("should_zip", "0");

	return q.toString();
}
