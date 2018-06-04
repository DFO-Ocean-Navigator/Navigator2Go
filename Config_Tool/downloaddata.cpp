#include "downloaddata.h"

#include <QUrlQuery>
/***********************************************************************************/
QString DownloadData::ToAPIURL() const {
	QUrlQuery q{"http://navigator.oceansdata.ca/subset/?"};
	q.addQueryItem("output_format", "NETCDF4");
	q.addQueryItem("variables", SelectedVariables.join(","));
	q.addQueryItem("dataset_name", ID);
	q.addQueryItem("time", StartDate.toString(Qt::DateFormat::ISODate) + "," + EndDate.toString(Qt::DateFormat::ISODate));

	return q.toString();
}
