#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>

/***********************************************************************************/
// Helper struct to return set preferences to mainwindow
struct Preferences {
	Preferences() noexcept = default;

	auto operator==(const Preferences& rhs) const noexcept {
		return ONInstallDir == rhs.ONInstallDir &&
				RemoteURL == rhs.RemoteURL &&
				UpdateRemoteListOnStart == rhs.UpdateRemoteListOnStart &&
				AutoStartServers == rhs.AutoStartServers &&
				IsOnline == rhs.IsOnline;
	}

	QString ONInstallDir;				// Ocean Navigator installation directory
	QString RemoteURL;
	bool UpdateRemoteListOnStart{true};	// Should we update dataset list from dory when starting
	bool AutoStartServers{false};		// Should we start gUnicorn and THREDDS on program start?
	bool IsOnline{true};				// Online or Offline? This determines the active config file.
};

#endif // PREFERENCES_H
