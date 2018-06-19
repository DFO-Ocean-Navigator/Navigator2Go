#include "network.h"

#include <QNetworkReply>
#include <QMessageBox>

#include <memory>

namespace Network {

/***********************************************************************************/
void MakeAPIRequest(QNetworkAccessManager& nam, const QString& APIURL, const std::function<void(QJsonDocument)> replyHandler, const std::function<void()> errorHandler) {

	const QNetworkRequest request{APIURL};
	// Send our request
	const auto reply = nam.get(request);

	auto context = std::make_unique<QObject>(new QObject);
	const auto pcontext = context.get();
	// Connect the "finished" signal from our reply
	// to the following lambda. This allows the Access Manager
	// to handle simultaneous requests
	QObject::connect(reply, &QNetworkReply::finished, pcontext,
		[context = std::move(context), replyHandler, errorHandler, reply]() mutable {
			context.reset(); // Clear context

			// Check for errors
			if (reply->error()) {
				QMessageBox::critical(	nullptr,
										QObject::tr("Network Error"),
										reply->errorString());

				if (errorHandler) {
					errorHandler();
				}

				return;
			}

			// Read raw data stream
			const auto data{ reply->readAll() };

			// Parse JSON response
			QJsonParseError error;
			const auto document{ QJsonDocument::fromJson(data, &error) };

			// Check for errors
			if (document.isNull() || error.error != QJsonParseError::NoError) {
				QMessageBox msgBox{nullptr};
				msgBox.setText(QObject::tr("API Request Error."));
				msgBox.setInformativeText(error.errorString());
				msgBox.setIcon(QMessageBox::Critical);
				msgBox.exec();

				return;
			}

			// Invoke our handler with supplied QJsonDocument
			replyHandler(document);

			// Mark request to be garbage collected
			reply->deleteLater();
		}
	);
}

/***********************************************************************************/
URLExistsRunnable::URLExistsRunnable(	const QString& urlString,
										const std::function<void()> successHandler,
										const std::function<void(const QString& errorString)> errorHandler) :	QRunnable{},
																												m_url{urlString},
																												m_successHandler{successHandler},
																												m_errorHandler{errorHandler} {}
/***********************************************************************************/
void URLExistsRunnable::run() {
	QTcpSocket socket;
	socket.connectToHost(m_url.host(), 80);

	if (socket.waitForConnected()) {
		socket.write("HEAD " + m_url.path().toUtf8() + " HTTP/1.1\r\n"
						"Host: " + m_url.host().toUtf8() + "\r\n\r\n");

		if (socket.waitForReadyRead()) {
			const auto bytes{ socket.readAll() };
			if (bytes.contains("200 OK")) {
				m_successHandler();
			}
		}
	}

	m_errorHandler(socket.errorString());
}

} // namespace Network
