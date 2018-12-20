#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <type_traits>

namespace Test {

/// Checks if a given function Func is marked noexcept.
template <typename Func, typename... Args>
struct is_noexcept : ::std::false_type {};

template <typename Func, typename... Args>
struct is_noexcept<Func(Args...) noexcept> : ::std::true_type {};

template <typename T> constexpr auto is_no_copy_no_move() noexcept {
  return !(::std::is_move_constructible_v<T> &&
           ::std::is_move_assignable_v<T> &&
           ::std::is_copy_constructible_v<T> && ::std::is_copy_assignable_v<T>);
}

} // namespace Test

#endif // HELPER_FUNCTIONS_H
