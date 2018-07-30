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
					std::function<void(QJsonDocument)>&& replyHandler,
					std::function<void()>&& errorHandler = nullptr
					);

/***********************************************************************************/
class URLExistsRunnable : public QObject, public QRunnable {
	Q_OBJECT
public:
	explicit URLExistsRunnable(const QString& urlString, const quint16 port);

	void run() override;

signals:
	void urlResult(const bool success);

private:
	const QUrl m_url;
	const quint16 m_port;
};

} // namespace Network

#endif // NETWORK_H
