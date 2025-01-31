module;

// std
#include <string>
#include <utility>

// external
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/result/result.hpp>
#include <mitama/thiserror/thiserror.hpp>

export module poac.util.result;

export namespace poac {

//
// Namespaces
//
namespace anyhow = mitama::anyhow;
namespace thiserror = mitama::thiserror;

//
// Data types
//
template <typename T, typename E = void>
using Result = std::conditional_t<
    std::is_void_v<E>, anyhow::result<T>, mitama::result<T, E>>;

// alias_fn(Ok, mitama::success); // NOLINT(readability-identifier-naming)

// NOLINT(readability-identifier-naming)
template <typename... Args>
inline auto Ok(Args&&... args)
    -> decltype(mitama::success(std::forward<Args>(args)...)) {
  return mitama::success(std::forward<Args>(args)...);
}

template <typename E = void, typename... Args>
inline auto Err(Args&&... args) { // NOLINT(readability-identifier-naming)
  if constexpr (std::is_void_v<E>) {
    return mitama::failure(std::forward<Args>(args)...);
  } else {
    return anyhow::failure<E>(std::forward<Args>(args)...);
  }
}

//
// Errors
//
template <thiserror::fixed_string S, class... T>
using Error = thiserror::error<S, T...>;

using SubprocessFailed =
    Error<"`{}` completed with exit code {}", std::string, std::int32_t>;

//
// Utilities
//
// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr auto to_anyhow = [](const std::string& e) {
  return anyhow::anyhow(e);
};

} // namespace poac
