#ifndef PROCESS_H
#define PROCESS_H

#include "nodiscard.h"

#include <QString>

namespace System {

NODISCARD bool IsProcessRunning(const QString& processName);

} // namespace System


#endif // PROCESS_H
