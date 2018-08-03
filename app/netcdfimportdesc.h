#ifndef NETCDFIMPORTDESC_H
#define NETCDFIMPORTDESC_H

#include "nodiscard.h"

#include <QString>

/***********************************************************************************/
/// Descriptor to import a netCDF file into THREDDS
struct NODISCARD NetCDFImportDesc {
	NetCDFImportDesc() noexcept = default;

	const QString File;	///< Filepath on disk
	const QString DatasetToAppendTo; ///< Dataset key to append to in THREDDS
};

#endif // NETCDFIMPORTDESC_H
