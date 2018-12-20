#include "datadownloaddesc.h"

#include <QUrlQuery>
/***********************************************************************************/
QString DataDownloadDesc::GetAPIQuery(const QString &remoteURL) const {
  QUrlQuery q{remoteURL + QStringLiteral("/subset/?")};
  q.addQueryItem(QStringLiteral("variables"),
                 SelectedVariables.join(QStringLiteral(",")));
  q.addQueryItem(QStringLiteral("quantum"), Quantum);
  q.addQueryItem(QStringLiteral("dataset_name"), ID);
  q.addQueryItem(QStringLiteral("time"),
                 StartDate.toString(Qt::DateFormat::ISODate) +
                     QStringLiteral(",") +
                     EndDate.toString(Qt::DateFormat::ISODate));
  q.addQueryItem(QStringLiteral("should_zip"), "0");

  return q.toString();
}
