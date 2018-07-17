#ifndef NETCDFIMPORTDESC_H
#define NETCDFIMPORTDESC_H

#include "nodiscard.h"

#include <QString>

/***********************************************************************************/
struct NODISCARD NetCDFImportDesc {
	NetCDFImportDesc() noexcept = default;

	const QString File;
	const QString DatasetToAppendTo;
};

#endif // NETCDFIMPORTDESC_H
