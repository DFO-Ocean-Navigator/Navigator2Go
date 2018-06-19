#ifndef NETWORK_H
#define NETWORK_H

#include <QString>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QRunnable>

#include <functional>

namespace Network {

/***********************************************************************************/
// Makes a GET request to a given API URL (usually Dory in our case), and handles
// error-checking. The given replyHandler is invoked on success.
void MakeAPIRequest(QNetworkAccessManager& nam,
					const QString& APIURL,
					const std::function<void(QJsonDocument)> replyHandler,
					const std::function<void()> errorHandler = nullptr
					);

/***********************************************************************************/
class URLExistsRunnable : public QObject, public QRunnable {
	Q_OBJECT
public:
	URLExistsRunnable(const QString& urlString);

	void run() override;

signals:
	void urlResult(const bool success);

private:
	const QUrl m_url;
};

} // namespace Network

#endif // NETWORK_H
