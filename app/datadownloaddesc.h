#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include "nodiscard.h"

#include <QDate>
#include <QStringList>

/***********************************************************************************/
/// Holds info needed to download a dataset from the remote server.
struct NODISCARD DataDownloadDesc {

  NODISCARD auto operator==(const DataDownloadDesc &rhs) const noexcept {
    return ID == rhs.ID;
  }

  /// Constructs an Ocean Navigator subset query from the member variables.
  NODISCARD QString GetAPIQuery(const QString &remoteURL) const;

  QString ID, Name, Quantum;
  QDate StartDate, EndDate;
  QStringList SelectedVariables;
};

/***********************************************************************************/
/// Overload qHash to allow storage in a QHash table.
NODISCARD inline auto qHash(const DataDownloadDesc &dlData) noexcept {
  return qHash(dlData.ID);
}

#endif // DOWNLOADDATA_H
