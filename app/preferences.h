#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>

/***********************************************************************************/
// Helper struct to return set preferences to mainwindow
struct Preferences {
	Preferences() noexcept = default;

	QString ONInstallDir;				// Ocean Navigator installation directory
	QString RemoteURL;
	QString THREDDSDataLocation;		// Location on disk of THREDDS datasets
	bool UpdateRemoteListOnStart{true};	// Should we update dataset list from dory when starting
	bool AutoStartServers{false};		// Should we start gUnicorn and THREDDS on program start?
	bool IsNetworkOnline{true};			// Online or Offline? This determines the active config file.
	bool DatasetTargetRemote{true};
};

#endif // PREFERENCES_H
