#include "apirequest.h"

#include <QNetworkReply>
#include <QMessageBox>

#include <memory>

/***********************************************************************************/
void MakeAPIRequest(QNetworkAccessManager& nam, const QString& APIURL, const std::function<void(QJsonDocument)> replyHandler) {

	const QNetworkRequest request{APIURL};
	// Send our request
	const auto reply = nam.get(request);

	auto context = std::make_unique<QObject>(new QObject);
	const auto pcontext = context.get();
	// Connect the "finished" signal from our reply
	// to the following lambda. This allows the Access Manager
	// to handle simultaneous requests
	QObject::connect(reply, &QNetworkReply::finished, pcontext,
		[context = std::move(context), replyHandler, reply]() mutable {
			context.reset(); // Clear context

			// Check for errors
			if (reply->error()) {
				QMessageBox::critical(	nullptr,
										QObject::tr("Network Error"),
										reply->errorString());
				return;
			}

			// Read raw data stream
			const auto data = reply->readAll();

			// Parse JSON response
			QJsonParseError error;
			const auto document = QJsonDocument::fromJson(data, &error);

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
