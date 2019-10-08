#include "jsonio.h"

#include <QFile>
#include <QMessageBox>
#include <QSaveFile>

#ifdef QT_DEBUG
#include <QDebug>
#endif

namespace IO {

/***********************************************************************************/
QJsonDocument LoadJSONFile(const QString &path, const bool showMsgBox) {
  // Try to open file
  QFile f{path};
  if (!f.open(QFile::ReadOnly | QFile::Text)) {
#ifdef QT_DEBUG
    qDebug() << "File Open Error: " << path;
    qDebug() << f.errorString();
#endif
    if (showMsgBox) {
      QMessageBox msgBox;
      msgBox.setWindowTitle(QObject::tr("Error..."));
      msgBox.setText(path);
      msgBox.setInformativeText(QObject::tr("File not found!"));
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
    }

    return QJsonDocument();
  }
  const QString &contents{f.readAll()}; // Get file contents
  f.close();

  // Parse json
  QJsonParseError error; // catch errors
  const auto &jsonDocument{QJsonDocument::fromJson(contents.toUtf8(), &error)};
  // Check for errors
  if (jsonDocument.isNull()) {
#ifdef QT_DEBUG
    qDebug() << "JSON Error: " << path;
    qDebug() << error.errorString();
#endif
    if (showMsgBox) {
      QMessageBox msgBox;
      msgBox.setWindowTitle(QObject::tr("Error parsing JSON file..."));
      msgBox.setText(path);
      msgBox.setInformativeText(QObject::tr("JSON syntax error detected."));
      msgBox.setDetailedText(error.errorString());
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
    }

    return QJsonDocument(); // Return empty doc
  }

  return jsonDocument;
}

/***********************************************************************************/
void WriteJSONFile(const QString &path, const QJsonObject &obj) {

  const QJsonDocument doc{obj};

  QSaveFile f{path};
  f.open(QFile::WriteOnly | QFile::Text |
         QFile::Truncate); // Overwrite original file.
  f.write(doc.toJson());
  f.commit();
}

} // namespace IO
