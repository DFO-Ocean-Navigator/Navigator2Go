#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QString>
#include <QJsonDocument>
#include <QNetworkAccessManager>

#include <functional>

/***********************************************************************************/
// Forward declarations
class QObject;

/***********************************************************************************/
void MakeAPIRequest(QNetworkAccessManager& nam, const QString& APIURL, std::function<void(QJsonDocument)> replyHandler);

#endif // APIREQUEST_H
