#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>

/***********************************************************************************/
// Helper struct to return set preferences to mainwindow
struct Preferences {
	QString ONInstallDir;	// Ocean Navigator installation directory
	QString ONDatasetConfig;	// Holds dataset config file
	bool UpdateDoryListOnStart; // Should we update dataset list from dory when starting
	bool AutoStartServers;		// Should we start gUnicorn and THREDDS on program start?
};

#endif // PREFERENCES_H
