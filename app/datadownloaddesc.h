#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include <QDate>
#include <QStringList>

/***********************************************************************************/
struct DataDownloadDesc {

	auto operator==(const DataDownloadDesc& rhs) const Q_DECL_NOTHROW {
		return ID == rhs.ID;
	}

	QString ToAPIURL() const;

	QString ID, Name;
	QDate StartDate, EndDate;
	QStringList SelectedVariables;
};

/***********************************************************************************/
inline auto qHash(const DataDownloadDesc& dlData) Q_DECL_NOTHROW {
	return qHash(dlData.ID);
}

#endif // DOWNLOADDATA_H
