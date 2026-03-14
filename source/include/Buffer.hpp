
#pragma once

#include <cstddef>
#include <cstdint>
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

class RGBBuffer final : public Buffer<uint8_t> {
  using Buffer::Buffer;
};

class Tile final {
public:
  Tile(size_t fromX, size_t fromY, size_t width, size_t height)
      : fromX_(fromX), fromY_(fromY), width_(width), height_(height) {}

  size_t fromX_, fromY_, width_, height_;
};
