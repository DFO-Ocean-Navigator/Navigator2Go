#ifndef PROCESS_H
#define PROCESS_H

#include "nodiscard.h"

#include <QString>

namespace System {

/// Queries the OS given a process name to check if it's running.
NODISCARD bool IsProcessRunning(const QString& processName);

} // namespace System


#endif // PROCESS_H
