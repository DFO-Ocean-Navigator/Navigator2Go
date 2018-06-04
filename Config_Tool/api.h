#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QString>
#include <QJsonDocument>
#include <QNetworkAccessManager>

#include <functional>

namespace API {

/***********************************************************************************/
// Makes a GET request to a given API URL (usually Dory in our case), and handles
// error-checking. The given replyHandler is invoked on success.
void MakeAPIRequest(QNetworkAccessManager& nam, const QString& APIURL, const std::function<void(QJsonDocument)> replyHandler);

} // namespace API

#endif // APIREQUEST_H
