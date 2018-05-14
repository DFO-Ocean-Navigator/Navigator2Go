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
// Makes a GET request to a given API URL (usually Dory in our case), and handles
// error-checking. The given replyHandler is invoked on success.
void MakeAPIRequest(QNetworkAccessManager& nam, const QString& APIURL, const std::function<void(QJsonDocument)> replyHandler);

#endif // APIREQUEST_H
