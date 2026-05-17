
#pragma once

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include <cstddef>
#include <tbb/parallel_for.h>

Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation, HittableList &world);

static inline uint8_t toByte(double col) { return int(col * 255.999); }

class CPURenderer {
public:
  CPURenderer(size_t imageWidth, size_t imageHeight, HittableList &world,
              Camera &cam, RGBBuffer &imageBuffer, size_t spp)
      : imageWidth_(imageWidth), imageHeight_(imageHeight), world_(world),
        cam_(cam), imageBuffer_(imageBuffer), spp_(spp) {

    splitIntoTiles(64, 64);
  }
  bool render() {

    tbb::parallel_for(size_t(0), tiles_.size(), [this](size_t i) {
      renderTiles(tiles_[i], cam_, world_);
    });
    return true;
  };

private:
  size_t imageWidth_;
  size_t imageHeight_;
  HittableList &world_;
  Camera &cam_;
  RGBBuffer &imageBuffer_;
  size_t spp_;
  std::vector<Tile> tiles_;

  void splitIntoTiles(size_t tileWidth, size_t tileHeight);
  void renderTiles(Tile &tile, Camera &cam, HittableList &world_);
};
