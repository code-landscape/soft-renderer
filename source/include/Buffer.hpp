
#pragma once

#include <cstddef>
#include <vector>

template <typename typeT> class Buffer {
public:
  Buffer(size_t size) { data_.resize(size); }
  virtual ~Buffer() = default;
  std::vector<typeT> data_;
  auto getData() { return data_.data(); }
  auto getSize() { return data_.size(); }
  auto &operator[](size_t i) { return data_[i]; }
};
