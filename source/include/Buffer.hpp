
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

template <typename typeT, size_t sizeT> class Buffer {
public:
  virtual ~Buffer() = default;

protected:
  std::array<typeT, sizeT> data_;
};

template <size_t imageWidthT, size_t imageHeightT>
class ImageBuffer final : public Buffer<uint8_t, imageWidthT * imageHeightT> {
public:
  void write() override final { this->data_; }
};
