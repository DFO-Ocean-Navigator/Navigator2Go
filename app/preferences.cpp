#include "preferences.h"

#include <QCoreApplication>

/***********************************************************************************/
Preferences::Preferences(QObject *parent)
    : QSettings{QSettings::IniFormat, QSettings::UserScope,
                QCoreApplication::instance()->organizationName(),
                QCoreApplication::instance()->applicationName(), parent} {
  readSettings();
}

/***********************************************************************************/
Preferences::~Preferences() { writeSettings(); }

/***********************************************************************************/
void Preferences::readSettings() {
  // General
  beginGroup(QStringLiteral("General"));

  if (!contains(QStringLiteral("FirstRun"))) {
    FirstRun = true;
  } else {
    FirstRun = false;
  }

  if (contains(QStringLiteral("ONInstallDir"))) {
    ONInstallDir = value(QStringLiteral("ONInstallDir")).toString();
  } else {
    ONInstallDir = QStringLiteral("/opt/Ocean-Data-Map-Project/");
  }

  if (contains(QStringLiteral("RemoteURL"))) {
    RemoteURL = value(QStringLiteral("RemoteURL")).toString();
  } else {
    RemoteURL = QStringLiteral("http://www.navigator.oceansdata.ca/");
  }

  if (contains(QStringLiteral("THREDDSCatalogLocation"))) {
    THREDDSCatalogLocation =
        value(QStringLiteral("THREDDSCatalogLocation")).toString();
  } else {
    THREDDSCatalogLocation = QStringLiteral("/opt/thredds_content/thredds");
  }

  if (contains(QStringLiteral("UpdateRemoteListOnStart"))) {
    UpdateRemoteListOnStart =
        value(QStringLiteral("UpdateRemoteListOnStart")).toBool();
  }

  if (contains(QStringLiteral("CheckForUpdatesOnStart"))) {
    CheckForUpdatesOnStart =
        value(QStringLiteral("CheckForUpdatesOnStart")).toBool();
  }

  if (contains(QStringLiteral("AdvancedUI"))) {
    AdvancedUI = value(QStringLiteral("AdvancedUI")).toBool();
  }

  endGroup();

  //
  beginGroup(QStringLiteral("DataOrder"));

  if (contains(QStringLiteral("DataDownloadFormat"))) {
    DataDownloadFormat = value(QStringLiteral("DataDownloadFormat")).toString();
  }

  if (contains("MinLat")) {
    DataOrderArea.MinLat = value(QStringLiteral("MinLat")).toDouble();
    DataOrderArea.MaxLat = value(QStringLiteral("MaxLat")).toDouble();
    DataOrderArea.MinLon = value(QStringLiteral("MinLon")).toDouble();
    DataOrderArea.MaxLon = value(QStringLiteral("MaxLon")).toDouble();
  } else {
    DataOrderArea.MinLat = 40.0;
    DataOrderArea.MaxLat = 47.0;
    DataOrderArea.MinLon = -68.0;
    DataOrderArea.MaxLon = -57.0;
  }

  endGroup();

  //
  beginGroup(QStringLiteral("Jobs"));
  endGroup();
}

/***********************************************************************************/
void Preferences::writeSettings() {
  // General
  beginGroup(QStringLiteral("General"));

  setValue(QStringLiteral("FirstRun"), false);
  setValue(QStringLiteral("ONInstallDir"), ONInstallDir);
  setValue(QStringLiteral("RemoteURL"), RemoteURL);
  setValue(QStringLiteral("THREDDSCatalogLocation"), THREDDSCatalogLocation);
  setValue(QStringLiteral("UpdateRemoteListOnStart"), UpdateRemoteListOnStart);
  setValue(QStringLiteral("CheckForUpdatesOnStart"), CheckForUpdatesOnStart);
  setValue(QStringLiteral("AdvancedUI"), AdvancedUI);

  endGroup();

  // DataOrder
  beginGroup(QStringLiteral("DataOrder"));
  setValue(QStringLiteral("MinLat"), DataOrderArea.MinLat);
  setValue(QStringLiteral("MaxLat"), DataOrderArea.MaxLat);
  setValue(QStringLiteral("MinLon"), DataOrderArea.MinLon);
  setValue(QStringLiteral("MaxLon"), DataOrderArea.MaxLon);

  setValue(QStringLiteral("DataDownloadFormat"), DataDownloadFormat);

  endGroup();

  // Jobs
  beginGroup(QStringLiteral("Jobs"));
  endGroup();
}
