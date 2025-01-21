#ifndef JESL_CLEANUP_H
#define JESL_CLEANUP_H

#include <type_traits>

namespace jesl {

template <typename C>
class Cleanup {
 public:
  Cleanup() = delete;
  Cleanup(C&& c) : cleanup_(std::move(c)) {}
  Cleanup(Cleanup<C>&& other) { *this = other; }
  Cleanup(const Cleanup& other) = delete;
  Cleanup& operator=(const Cleanup& other) = delete;
  Cleanup& operator=(Cleanup&& other) {
    release();
    cleanup_ = std::move(other.cleanup_);
    docleanup_ = true;
    other.release();
  }

  ~Cleanup() {
    if constexpr (std::is_invocable_v<C>) {
      if (docleanup_) {
        cleanup_();
      }
    }
  }

  // Causes the cleanup to be released
  void release() {
    cleanup_ = {};
    docleanup_ = false;
  }

 private:
  C cleanup_;
  bool docleanup_ = true;
};

template <typename Callable, typename... Args>
auto CallWhenDestroyed(Args&&... args) {
  return Cleanup([args...]() { Callable(args...); });
}

}  // namespace jesl

#endif  // JESL_CLEANUP_H