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

  void AssertLocked() {
    configASSERT(xSemaphoreGetMutexHolder(mutex_.get()) ==
                 xTaskGetCurrentTaskHandle());
  }
  void AssertUnlocked() {
    configASSERT(xSemaphoreGetMutexHolder(mutex_.get()) == nullptr);
  }

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

template <typename T>
class Borrowable;

// Borrowed and Borrowable provide uniform mechanisms for wrapping a type in
// a Mutex. Finer grained locking is sometimes possible or desirable, but if
// you want to turn a thread unsafe type into a thread safe one this is a
// generic approach.
template <typename T>
class Borrowed {
 public:
  ~Borrowed() {
    if (parent_) parent_->Unlock();
  }

  Borrowed(Borrowed&& o) {
    parent_ = o.parent_;
    o.parent_ = nullptr;
  }
  Borrowed& operator=(Borrowed&& o) {
    parent_ = o.parent_;
    o.parent_ = nullptr;
    return *this;
  }

  // The lifetime of any pointers passed from here should never exceed the
  // lifetime of the Borrowed object.
  T* operator->();
  T& operator*();
  const T* operator->() const;
  const T& operator*() const;

 private:
  Borrowable<T>* parent_;
};

template <typename T>
class Borrowable {
 public:
  template <typename... Args>
  Borrowable(Args&&... args) : value_(std::forward<Args>(args)...) {}
  ~Borrowable() { mu_.AssertUnlocked(); }

  Borrowable(const Borrowable&) = delete;
  Borrowable(Borrowable&&) = delete;
  Borrowable& operator=(const Borrowable&) = delete;
  Borrowable& operator=(Borrowable&&) = delete;

  Borrowed<T> Borrow() {
    mu_.Lock();
    Borrowed<T> borrowed;
    borrowed.parent_ = this;
    return borrowed;
  }

 private:
  T value_;
  Mutex mu_;
};

template <typename T>
T* Borrowed<T>::operator->() {
  configASSERT(parent_);
  return &parent_->value_;
}
template <typename T>
T& Borrowed<T>::operator*() {
  configASSERT(parent_);
  return parent_->value_;
}
template <typename T>
const T* Borrowed<T>::operator->() const {
  configASSERT(parent_);
  return &parent_->value_;
}
template <typename T>
const T& Borrowed<T>::operator*() const {
  configASSERT(parent_);
  return parent_->value_;
}

}  // namespace jesl

#endif  // JESL_FREERTOS_MUTEX_H