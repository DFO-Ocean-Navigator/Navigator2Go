#ifndef DEFINES_H
#define DEFINES_H

#if __has_cpp_attribute(nodiscard)
	#define NODISCARD [[nodiscard]]
#elif __has_cpp_attribute(gnu::warn_unused_result)
	#define NODISCARD __attribute__((warn_unused_result))
#else
	#define NODISCARD
#endif

/***********************************************************************************/
// Timeout for status bar messages
const constexpr int STATUS_BAR_MSG_TIMEOUT{2000};

/***********************************************************************************/
// Colours
const constexpr auto COLOR_GREEN{"color: rgb(115, 210, 22);"};
const constexpr auto COLOR_RED{"color: rgb(239, 41, 41);"};

#endif // DEFINES_H
