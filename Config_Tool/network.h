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
class URLExistsRunnable : public QRunnable {

public:
	URLExistsRunnable(const QString& urlString,
					  const std::function<void()> successHandler,
					  const std::function<void(const QString&)> errorHandler);

	void run() override;

private:
	const QUrl m_url;
	const std::function<void()> m_successHandler{nullptr};
	const std::function<void(const QString&)> m_errorHandler{nullptr};
};

} // namespace Network

#endif // NETWORK_H
