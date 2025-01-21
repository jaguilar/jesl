#ifndef JESL_FREERTOS_QUEUE_H
#define JESL_FREERTOS_QUEUE_H

#include <expected>
#include <memory>
#include <type_traits>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

namespace jesl {

namespace internal {

template <int ObjectSize>
class RTOSQueueBase {
 public:
  explicit RTOSQueueBase(int length)
      : queue_(xQueueCreate(length, ObjectSize)) {}
  ~RTOSQueueBase() = default;
  RTOSQueueBase(const RTOSQueueBase&) = delete;
  RTOSQueueBase(RTOSQueueBase&&) = default;
  RTOSQueueBase& operator=(const RTOSQueueBase&) = delete;
  RTOSQueueBase& operator=(RTOSQueueBase&&) = default;

  bool Send(const void* item, TickType_t ticks_to_wait = portMAX_DELAY) {
    return xQueueSend(queue_.get(), item, ticks_to_wait) == pdTRUE;
  }

  BaseType_t Receive(void* item, TickType_t ticks_to_wait = portMAX_DELAY) {
    return xQueueReceive(queue_.get(), item, ticks_to_wait) == pdTRUE;
  }

 private:
  struct FreeQueueHandle {
    void operator()(QueueHandle_t queue) { vQueueDelete(queue); }
  };
  std::unique_ptr<std::remove_pointer<QueueHandle_t>::type, FreeQueueHandle>
      queue_;
};

}  // namespace internal

template <typename T>
class RTOSQueue {
 public:
  static_assert(std::is_trivially_copyable_v<T>);
  using Base = internal::RTOSQueueBase<sizeof(T)>;

  RTOSQueue(int length) : base_(length) {}
  ~RTOSQueue() = default;

  bool Send(const T& item, TickType_t ticks_to_wait = portMAX_DELAY) {
    return base_.Send(&item, ticks_to_wait) == pdTRUE;
  }

  std::expected<T, BaseType_t> Receive(
      TickType_t ticks_to_wait = portMAX_DELAY) {
    T item;
    BaseType_t result = base_.Receive(&item, ticks_to_wait);
    if (result == pdTRUE) {
      return item;
    } else {
      return std::unexpected(result);
    }
  }

 private:
  Base base_;
};

}  // namespace jesl

#endif