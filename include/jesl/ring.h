#ifndef JESL_RING_H
#define JESL_RING_H

#include <array>
#include <cstdint>
#include <deque>
#include <iterator>

namespace jesl {

template <typename T, std::size_t N>
class Ring {
 public:
  void insert(T&& t) {
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

 private:
  // buf_ grows backwards so we can iterator forwards.
  std::array<T, N> buf_;
  int head_ = 0;
  int nset_ = 0;
};

}  // namespace jesl

#endif