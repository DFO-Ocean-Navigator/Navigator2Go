#include "datadownloaddesc.h"

#include <QUrlQuery>
/***********************************************************************************/
QString DataDownloadDesc::ToAPIURL() const {
	QUrlQuery q{"http://localhost:5000/subset/?"};
	q.addQueryItem("output_format", "NETCDF4");
	q.addQueryItem("variables", SelectedVariables.join(","));
	q.addQueryItem("dataset_name", ID);
	q.addQueryItem("time", StartDate.toString(Qt::DateFormat::ISODate) + "," + EndDate.toString(Qt::DateFormat::ISODate));
	q.addQueryItem("should_zip", "0");

	return q.toString();
}
