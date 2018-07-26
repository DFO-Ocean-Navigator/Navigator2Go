#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include "nodiscard.h"

#include <QDate>
#include <QStringList>

/***********************************************************************************/
struct NODISCARD DataDownloadDesc {

	NODISCARD auto operator==(const DataDownloadDesc& rhs) const Q_DECL_NOTHROW {
		return ID == rhs.ID;
	}

	NODISCARD QString GetAPIQuery(const QString& remoteURL) const;

	QString ID, Name;
	QDate StartDate, EndDate;
	QStringList SelectedVariables;
};

/***********************************************************************************/
NODISCARD inline auto qHash(const DataDownloadDesc& dlData) Q_DECL_NOTHROW {
	return qHash(dlData.ID);
}

#endif // DOWNLOADDATA_H
