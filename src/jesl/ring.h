#ifndef JESL_RING_H
#define JESL_RING_H

#include <array>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iterator>

namespace jesl {

template <typename T, std::size_t N>
class Ring {
 public:
  // Returns the number of elements that can be stored.
  static constexpr auto capacity() { return N; }

  // Returns the number of elements that have been added if less than
  // capacity(), or else capacity().
  std::size_t size() const { return nset_; }

  // Inserts one element. If size() == capacity(), overwrites the oldest
  // element.
  void insert(T t) {
    buf_[head_++] = std::move(t);
    if (head_ >= buf_.size()) {
      head_ = 0;
    }
    if (nset_ < buf_.size()) ++nset_;
  }

  class const_iterator {
   public:
    using difference_type = int;
    using value_type = T;

    const_iterator() : r_(nullptr), offset_(-1) {}
    const_iterator(Ring* r) : r_(r), offset_(r_->nset_) {}
    const_iterator(const const_iterator& o) = default;
    T operator*() const {
      int idx = r_->head_ - offset_;
      if (idx < 0) {
        idx += r_->buf_.size();
      }
      return r_->buf_[idx];
    }
    const_iterator& operator++() {
      --offset_;
      if (offset_ <= 0) {
        offset_ = -1;
        r_ = nullptr;
      }
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator cpy = *this;
      ++*this;
      return cpy;
    }
    bool operator==(const const_iterator& o) const {
      return offset_ == o.offset_ && r_ == o.r_;
    }
    bool operator!=(const const_iterator& o) const { return !(*this == o); }

   private:
    Ring* r_;
    int offset_;
  };
  static_assert(std::forward_iterator<const_iterator>);

  const_iterator begin() { return const_iterator(this); }
  const_iterator end() { return const_iterator(); }

  // Returns the most recently inserted element.
  const T& back() {
    assert(nset_ > 0);
    int idx = head_ - 1;
    if (idx < 0) idx += buf_.size();
    return buf_[idx];
  }

  // Returns the least recently inserted element.
  const T& front() {
    assert(nset_ > 0);
    int idx = head_ - nset_;
    if (idx < 0) idx += buf_.size();
    return buf_[idx];
  }

 private:
  std::array<T, N> buf_;
  int head_ = 0;
  int nset_ = 0;
};

}  // namespace jesl

#endif