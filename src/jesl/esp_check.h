#ifndef JESL_ESP_CHECK_H
#define JESL_ESP_CHECK_H

#include <esp_check.h>

#include <expected>
#include <optional>
#include <type_traits>

#include "esp_err.h"

namespace jesl::detail {
// Extracts the error code directly, returning ESP_OK if no error exists.
// This avoids the overhead of constructing std::optional.
template <typename T>
constexpr esp_err_t ExtractError(T&& val) {
  using DecayedT = std::remove_cvref_t<T>;

  if constexpr (std::is_same_v<DecayedT, esp_err_t>) {
    return val;
  } else {
    if (!val) {
      return val.error();
    }
    return ESP_OK;
  }
}

}  // namespace jesl::detail

#define ESP_RETURN_IF_ERROR(expr)                                \
  do {                                                           \
    auto&& _jesl_res = (expr);                                   \
    esp_err_t _jesl_err = jesl::detail::ExtractError(_jesl_res); \
    if (_jesl_err != ESP_OK) {                                   \
      return std::unexpected(_jesl_err);                         \
    }                                                            \
  } while (0)

#define ESP_ASSERT_OK(expr) ESP_ERROR_CHECK(jesl::detail::ExtractError(expr))

#define JESL_CONCAT_INNER(x, y) x##y
#define JESL_CONCAT(x, y) JESL_CONCAT_INNER(x, y)
#define ESP_ASSIGN_OR_RETURN_IMPL(res_name, lhs, expr)   \
  auto&& res_name = (expr);                              \
  esp_err_t JESL_CONCAT(res_name, _err) =                \
      jesl::detail::ExtractError(res_name);              \
  if (JESL_CONCAT(res_name, _err) != ESP_OK) {           \
    return std::unexpected(JESL_CONCAT(res_name, _err)); \
  }                                                      \
  lhs = std::move(*res_name)

#define ESP_MUST_ASSIGN_IMPL(res_name, lhs, expr)      \
  auto&& res_name = (expr);                            \
  ESP_ASSERT_OK(jesl::detail::ExtractError(res_name)); \
  lhs = std::move(*res_name);

#define ESP_ASSIGN_OR_RETURN(lhs, expr) \
  ESP_ASSIGN_OR_RETURN_IMPL(JESL_CONCAT(_jesl_res_, __LINE__), lhs, expr)
#define ESP_MUST_ASSIGN(lhs, expr) \
  ESP_MUST_ASSIGN_IMPL(JESL_CONCAT(_jesl_res_, __LINE__), lhs, expr)

#endif  // JESL_ESP_CHECK_H
