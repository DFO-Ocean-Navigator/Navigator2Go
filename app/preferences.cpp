#include "preferences.h"

#include <QCoreApplication>

/***********************************************************************************/
Preferences::Preferences(QObject* parent) :	QSettings{
												QSettings::IniFormat,
												QSettings::UserScope,
												QCoreApplication::instance()->organizationName(),
												QCoreApplication::instance()->applicationName(),
												parent} {
	readSettings();
}

/***********************************************************************************/
Preferences::~Preferences() {
	writeSettings();
}

/***********************************************************************************/
void Preferences::readSettings() {
	// General
	beginGroup("General");

	if (!contains("FirstRun")) {
		FirstRun = true;
	}
	else {
		FirstRun = false;
	}

	if (contains("ONInstallDir")) {
		ONInstallDir = value("ONInstallDir").toString();
	}
	else {
		ONInstallDir = "/opt/Ocean-Data-Map-Project/";
	}

	if (contains("RemoteURL")) {
		RemoteURL = value("RemoteURL").toString();
	}
	else {
		RemoteURL = "http://www.navigator.oceansdata.ca/";
	}

	if (contains("THREDDSCatalogLocation")) {
		THREDDSCatalogLocation = value("THREDDSCatalogLocation").toString();
	}
	else {
		THREDDSCatalogLocation = "/opt/thredds_content/thredds";
	}

	if (contains("UpdateRemoteListOnStart")) {
		UpdateRemoteListOnStart = value("UpdateRemoteListOnStart").toBool();
	}

	if (contains("IsNetworkOnline")) {
		IsNetworkOnline = value("IsNetworkOnline").toBool();
	}

	if (contains("CheckForUpdatesOnStart")) {
		CheckForUpdatesOnStart = value("CheckForUpdatesOnStart").toBool();
	}

	if (contains("AdvancedUI")) {
		AdvancedUI = value("AdvancedUI").toBool();
	}

	endGroup();

	//
	beginGroup("DataOrder");

	if (contains("DataDownloadFormat")) {
		DataDownloadFormat = value("DataDownloadFormat").toString();
	}

	if (contains("MinLat")) {
		DataOrderArea.MinLat = value("MinLat").toDouble();
		DataOrderArea.MaxLat = value("MaxLat").toDouble();
		DataOrderArea.MinLon = value("MinLon").toDouble();
		DataOrderArea.MaxLon = value("MaxLon").toDouble();
	}
	else {
		DataOrderArea.MinLat = 40.0;
		DataOrderArea.MaxLat = 47.0;
		DataOrderArea.MinLon = -68.0;
		DataOrderArea.MaxLon = -57.0;
	}

	endGroup();

	//
	beginGroup("Jobs");
	endGroup();
}

/***********************************************************************************/
void Preferences::writeSettings() {
	// General
	beginGroup("General");

	setValue("FirstRun", false);
	setValue("ONInstallDir", ONInstallDir);
	setValue("RemoteURL", RemoteURL);
	setValue("THREDDSCatalogLocation", THREDDSCatalogLocation);
	setValue("UpdateRemoteListOnStart", UpdateRemoteListOnStart);
	setValue("IsNetworkOnline", IsNetworkOnline);
	setValue("CheckForUpdatesOnStart", CheckForUpdatesOnStart);
	setValue("AdvancedUI", AdvancedUI);

	endGroup();

	// DataOrder
	beginGroup("DataOrder");
	setValue("MinLat", DataOrderArea.MinLat);
	setValue("MaxLat", DataOrderArea.MaxLat);
	setValue("MinLon", DataOrderArea.MinLon);
	setValue("MaxLon", DataOrderArea.MaxLon);

	setValue("DataDownloadFormat", DataDownloadFormat);

	endGroup();

	// Jobs
	beginGroup("Jobs");
	endGroup();
}
