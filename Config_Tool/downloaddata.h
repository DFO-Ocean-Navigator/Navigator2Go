#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include <QDate>
#include <QList>

/***********************************************************************************/
// Forward Declarations
class QListWidgetItem;

/***********************************************************************************/
struct DownloadData {

	auto operator==(const DownloadData& rhs) const Q_DECL_NOTHROW {
		return ID == rhs.ID;
	}

	QString ID, Name;
	QDate StartDate, EndDate;
	QList<QListWidgetItem*> SelectedVariables;
};

/***********************************************************************************/
inline uint qHash(const DownloadData& dlData) Q_DECL_NOTHROW {
	return qHash(dlData.ID);
}

#endif // DOWNLOADDATA_H
