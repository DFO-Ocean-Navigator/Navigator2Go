#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "nodiscard.h"

#include <QString>

/***********************************************************************************/
/// Contains the loaded and modified settings.
/** Move-only type. */
struct NODISCARD Preferences {
	Preferences() = default;

	Preferences(Preferences&&) = default;
	Preferences(const Preferences&) = delete;
	Preferences& operator=(Preferences&&) = default;
	Preferences& operator=(const Preferences&) = delete;

	QString ONInstallDir;				///< Ocean Navigator installation directory
	QString RemoteURL;					///< URL of remote Navigator server
	QString THREDDSCatalogLocation;		///< Location on disk of THREDDS datasets
	QString DataDownloadFormat{"NETCDF4"};	///< Format to download netCDF files: same values as on Ocean Navigator web gui.
	bool UpdateRemoteListOnStart{true};	///< Should we update dataset list from dory when starting
	bool AutoStartServers{false};		///< Should we start gUnicorn and THREDDS on program start?
	bool IsNetworkOnline{true};			///< Global network state (online or offline).
	bool CheckForUpdatesOnStart{true};	///< Check for program updates on start.
	bool FirstRun{true};				///< First time Navigator2Go is run?
};

#endif // PREFERENCES_H
