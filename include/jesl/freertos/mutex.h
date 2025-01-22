#ifndef JESL_FREERTOS_MUTEX_H
#define JESL_FREERTOS_MUTEX_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <memory>
#include <type_traits>

namespace jesl {

class Mutex {
 public:
  Mutex() : mutex_(xSemaphoreCreateMutex()) {}

  void Lock() { xSemaphoreTake(mutex_.get(), portMAX_DELAY); }
  void Unlock() { xSemaphoreGive(mutex_.get()); }

 private:
  struct SemaphoreDeleter {
    void operator()(void* args) {
      vSemaphoreDelete(static_cast<SemaphoreHandle_t>(args));
    }
  };
  std::unique_ptr<std::remove_pointer<SemaphoreHandle_t>::type,
                  SemaphoreDeleter>
      mutex_;
};

class ScopedLock {
 public:
  ScopedLock(Mutex& mu) : mu_(mu) { mu_.Lock(); }
  ~ScopedLock() { mu_.Unlock(); }
  ScopedLock(const ScopedLock&) = delete;
  ScopedLock(ScopedLock&&) = delete;
  ScopedLock& operator=(const ScopedLock&) = delete;
  ScopedLock& operator=(ScopedLock&&) = delete;

 private:
  Mutex& mu_;
};

}  // namespace jesl

#endif  // JESL_FREERTOS_MUTEX_H