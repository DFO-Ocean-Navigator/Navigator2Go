#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "nodiscard.h"

#include <QString>
#include <QSettings>

/***********************************************************************************/
struct NODISCARD BoundingBox {
	double MinLat, MaxLat;
	double MinLon, MaxLon;
};

/***********************************************************************************/
/// Contains the loaded and modified settings.
/** Move-only type. */
class Preferences : public QSettings {
	Q_OBJECT

public:
	explicit Preferences(QObject* parent = nullptr);
	~Preferences();

	Q_DISABLE_COPY(Preferences)

	QString ONInstallDir;				///< Ocean Navigator installation directory
	QString RemoteURL;					///< URL of remote Navigator server
	QString THREDDSCatalogLocation;		///< Location on disk of THREDDS datasets
	QString DataDownloadFormat{"NETCDF4"};	///< Format to download netCDF files: same values as on Ocean Navigator web gui.
	bool UpdateRemoteListOnStart{true};	///< Should we update dataset list from dory when starting
	bool CheckForUpdatesOnStart{true};	///< Check for program updates on start.
	bool FirstRun{true};				///< First time Navigator2Go is run?
	bool AdvancedUI{false};				///< Enable advanced UI features?
	BoundingBox DataOrderArea;

private:
	/// Read settings from disk into member vars.
	void readSettings();
	/// Write settings from member vars to disk.
	void writeSettings();
};

#endif // PREFERENCES_H
