#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "nodiscard.h"

#include <QString>

/***********************************************************************************/
// Helper struct to return set preferences to mainwindow
struct NODISCARD Preferences {
	Preferences() = default;

	QString ONInstallDir;				// Ocean Navigator installation directory
	QString RemoteURL;
	QString THREDDSCatalogLocation;		// Location on disk of THREDDS datasets
	QString DataDownloadFormat{"NETCDF4"};
	bool UpdateRemoteListOnStart{true};	// Should we update dataset list from dory when starting
	bool AutoStartServers{false};		// Should we start gUnicorn and THREDDS on program start?
	bool IsNetworkOnline{true};			// Global network state (online or offline).
	bool CheckForUpdatesOnStart{true};	// Check for program updates on start.
};

#endif // PREFERENCES_H
