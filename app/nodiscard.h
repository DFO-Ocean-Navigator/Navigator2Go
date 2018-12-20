#ifndef NODISCARD_H
#define NODISCARD_H

/// Preprocessor checks for nodiscard attribute support
#if __has_cpp_attribute(nodiscard)
#define NODISCARD [[nodiscard]]
#elif __has_cpp_attribute(gnu::warn_unused_result)
#define NODISCARD __attribute__((warn_unused_result))
#else
#define NODISCARD
#endif

#endif // NODISCARD_H
