#ifndef DEFINES_H
#define DEFINES_H

/***********************************************************************************/
/// Duration in ms a message is shown in the status bar of MainWindow
const constexpr int STATUS_BAR_MSG_TIMEOUT{2000};

/***********************************************************************************/
const constexpr auto QSS_COLOR_GREEN{ "color: rgb(115, 210, 22);" };
const constexpr auto QSS_COLOR_RED{ "color: rgb(239, 41, 41);" };

/***********************************************************************************/
const constexpr int COLOR_GREEN{ 115<<16 | 210<<8 | 22 };
const constexpr int COLOR_RED{ 239<<16 | 41<<8 | 41 };

#endif // DEFINES_H
