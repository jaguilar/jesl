#ifndef JESL_PIN_GUARD_H
#define JESL_PIN_GUARD_H

#include <bitset>
#include <cassert>
#include <optional>

#ifdef ESP32
#include <esp-hal-gpio.h>
#endif

namespace jesl {

// Guards pins an an embedded system. Prevents multiple objects from using
// the same pin.
template <int pin_count>
class PinGuard {
 public:
  class Token {
   public:
    Token(PinGuard& guard, int pin) : guard_(&guard), pin_(pin) {}
    ~Token() {
      if (pin_ >= 0) {
        assert(guard_);
        guard_->Release(pin_);
      }
    }
    Token(Token&& other) : guard_(other.guard_), pin_(other.pin_) {
      other.pin_ = -1;
    }
    Token(const Token& other) = delete;
    Token& operator=(const Token& other) = delete;
    Token& operator=(Token&& other) {
      if (pin_ >= 0) {
        assert(guard_);
        guard_->Release(pin_);
      }
      guard_ = other.guard_;
      pin_ = other.pin_;
      other.pin_ = -1;
      return *this;
    }

    operator int() const { return pin_; }

#if defined(ESP32)
    operator gpio_num_t() const { return (gpio_num_t)pin_; }
#endif

   private:
    PinGuard* guard_;
    int pin_;
  };

  PinGuard() : pins_(0) {}

  std::optional<Token> Acquire(int pin) {
    if (pins_.test(pin)) {
      return std::nullopt;
    }
    pins_.set(pin);
    return Token(*this, pin);
  }

 private:
  void Release(int pin) { pins_.reset(pin); }

  std::bitset<pin_count> pins_;
};

#if defined(NUM_DIGITAL_PINS) && !defined(JESL_PINGUARD_DEFAULT_COUNT)
#define JESL_PINGUARD_DEFAULT_COUNT NUM_DIGITAL_PINS
#endif

#if !defined(JESL_PINGUARD_DEFAULT_COUNT)
#define JESL_PINGUARD_DEFAULT_COUNT 0
#endif

#if JESL_PINGUARD_DEFAULT_COUNT > 0
PinGuard<JESL_PINGUARD_DEFAULT_COUNT>& DefaultPinGuard() {
  static PinGuard<JESL_PINGUARD_DEFAULT_COUNT> instance;
  return instance;
};
#endif

}  // namespace jesl

#endif
