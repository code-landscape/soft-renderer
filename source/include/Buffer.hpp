
#pragma once

#include <cstddef>
#include <vector>

template <typename typeT> class Buffer {
  std::vector<typeT> data_;

public:
  Buffer(size_t size) { data_.resize(size); }
  virtual ~Buffer() = default;
  auto getRawData() { return data_.data(); }
  auto &getData() { return data_; }
  auto getSize() { return data_.size(); }
  auto &operator[](size_t i) { return data_[i]; }
};
