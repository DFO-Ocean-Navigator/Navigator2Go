#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>

/***********************************************************************************/
// Helper struct to return set preferences to mainwindow
struct Preferences {
	QString ONInstallDir;	// Ocean Navigator installation directory
	QString ONActiveDatasetConfig;	// Holds active dataset config file
	bool UpdateDoryListOnStart; // Should we update dataset list from dory when starting
};

#endif // PREFERENCES_H
