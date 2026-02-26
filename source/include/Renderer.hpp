
#pragma once

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"

Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation, HittableList &world);

inline uint8_t toByte(double col) { return int(col * 255.999); }

template <size_t imageWidthT, size_t imageHeightT> class CPURenderer {
public:
  CPURenderer(HittableList &world, Camera &cam,
              RGBBuffer<imageWidthT, imageHeightT> &imageBuffer)
      : world_(world), cam_(cam), imageBuffer_(imageBuffer) {}
  bool render() {

    for (size_t y{0}; y != imageHeightT; y++) {
      for (size_t x{0}; x != imageWidthT; x++) {
        Vec3 attenuation{1, 1, 1};
        Vec3 color = rayColor(0, cam_.getRay(x, y), attenuation, world_);
        size_t index = (y * imageWidthT + x) * 3;
        imageBuffer_[index + 0] = toByte(color.r);
        imageBuffer_[index + 1] = toByte(color.g);
        imageBuffer_[index + 2] = toByte(color.b);
      }
    }

    return true;
  };

private:
  HittableList &world_;
  Camera &cam_;
  RGBBuffer<imageWidthT, imageHeightT> &imageBuffer_;
};
