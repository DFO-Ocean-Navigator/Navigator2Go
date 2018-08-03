#ifndef JSONIO_H
#define JSONIO_H

#include "nodiscard.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace IO {
/***********************************************************************************/
/// Loads a JSON file from disk, checks for errors, and returns the QJsonDocument.
NODISCARD QJsonDocument LoadJSONFile(const QString& path, const bool showMsgBox = true);

/***********************************************************************************/
/// Writes a JSON file to the given path
void WriteJSONFile(const QString& path, const QJsonObject& obj = QJsonObject());

} // namespace IO

#endif // JSONIO_H
