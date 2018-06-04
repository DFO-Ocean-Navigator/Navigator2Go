#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include <QDate>
#include <QStringList>

/***********************************************************************************/
struct DownloadData {

	auto operator==(const DownloadData& rhs) const Q_DECL_NOTHROW {
		return ID == rhs.ID;
	}

	QString ToAPIURL() const;

	QString ID, Name;
	QDate StartDate, EndDate;
	QStringList SelectedVariables;
};

/***********************************************************************************/
inline auto qHash(const DownloadData& dlData) Q_DECL_NOTHROW {
	return qHash(dlData.ID);
}

#endif // DOWNLOADDATA_H
