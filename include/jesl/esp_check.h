#ifndef JESL_ESP_CHECK_H
#define JESL_ESP_CHECK_H

#include "esp_err.h"

#define ESP_RETURN_IF_ERROR(expr) \
  do {                            \
    if (!(expr)) {                \
      return expr.error();        \
    }                             \
  } while (0)

// Type-safe error wrapper.
class EspError {
 public:
  EspError(esp_err_t err) : err_(err) {}
  operator esp_err_t() const { return err_; }

 private:
  esp_err_t err_;
};

#endif  // JESL_ESP_CHECK_H