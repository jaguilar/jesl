#ifndef JESL_NODESTRUCTOR_H
#define JESL_NODESTRUCTOR_H

#include <new>

namespace jesl {

// This funnily-named class is for putting objects that require construction
// into a static/global variable. It's not possible to delete the object
// once it has been constructed.
template <typename T>
class NoDestructor {
 public:
  NoDestructor() {}

  NoDestructor(const NoDestructor&) = delete;
  NoDestructor& operator=(const NoDestructor&) = delete;
  NoDestructor(NoDestructor&&) = delete;
  NoDestructor& operator=(NoDestructor&&) = delete;

  template <typename... Args>
  void Init(Args&&... args) {
    new (storage_) T(std::forward<Args>(args)...);
#ifndef NDEBUG
    initialized_ = true;
#endif
  }

  T* operator->() { return &get(); }
  T& operator*() { return get(); }
  operator T*() { return &get(); }
  operator T&() { return get(); }
  const T* operator->() const { return &get(); }
  const T& operator*() const { return get(); }
  operator const T*() { return &get(); }
  operator const T&() { return get(); }

  T* get() {
#ifndef NDEBUG
    assert(initialized_);
#endif
    return std::launder(reinterpret_cast<T*>(storage_));
  }
  const T* get() const {
#ifndef NDEBUG
    assert(initialized_);
#endif
    return std::launder(reinterpret_cast<const T*>(storage_));
  }

 private:
    alignas(alignof(T)) char storage_[sizeof(T)];
#ifndef NDEBUG
  bool initialized_ = false;
#endif
};

}  // namespace jesl

#endif