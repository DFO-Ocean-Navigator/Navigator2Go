#ifndef DEFINES_H
#define DEFINES_H

#if __has_cpp_attribute(nodiscard)
	#define NODISCARD [[nodiscard]]
#error "ERROR"
#elif __has_cpp_attribute(gnu::warn_unused_result)
	#define NODISCARD __attribute__((warn_unused_result))
#else
	#define NODISCARD
#endif

#endif // DEFINES_H
