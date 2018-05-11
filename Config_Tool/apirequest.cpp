#include "apirequest.h"

#include <QNetworkReply>
#include <QMessageBox>

#include <memory>

/***********************************************************************************/
void MakeAPIRequest(QNetworkAccessManager& nam, const QString& APIURL, std::function<void (QJsonDocument)> replyHandler) {

	const QNetworkRequest request{APIURL};

	auto context = std::make_unique<QObject>(new QObject);
	auto pcontext = context.get();

	QObject::connect(&nam, &QNetworkAccessManager::finished, pcontext,
		[&, context = std::move(context), replyHandler] (auto* reply) mutable {
			context.reset(); // Clear context

			// Check for errors
			if (reply->error()) {
				QMessageBox::critical(	nullptr,
										QObject::tr("Network Error"),
										reply->errorString());
				return;
			}

			const auto data = reply->readAll();

			// Parse JSON response
			QJsonParseError error;
			const auto document = QJsonDocument::fromJson(data, &error);
			// Check for errors
			if (document.isNull()) {
				QMessageBox msgBox{nullptr};
				msgBox.setText(QObject::tr("API Request Error."));
				msgBox.setInformativeText(error.errorString());
				msgBox.setIcon(QMessageBox::Critical);
				msgBox.exec();

				return;
			}

			replyHandler(document);


			// Mark request to be garbage collected
			reply->deleteLater();
		}
	);

	nam.get(request);
}