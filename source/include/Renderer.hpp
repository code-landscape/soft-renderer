
#pragma once

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Ray.hpp"
#include "pcg/pcg_extras.hpp"
#include "pcg/pcg_random.hpp"
#include <cstddef>
#include <random>
#include <tbb/parallel_for.h>

static inline uint8_t toByte(double col) { return int(col * 255.999); }

class Tile final {
public:
  Tile(size_t fromX, size_t fromY, size_t width, size_t height)
      : fromX_(fromX), fromY_(fromY), width_(width), height_(height) {}

  size_t fromX_, fromY_, width_, height_;
};

class CPURenderer {
public:
  CPURenderer(size_t imageWidth, size_t imageHeight, HittableList &world,
              Camera &cam, Buffer<uint8_t> &imageBuffer, size_t spp)
      : imageWidth_(imageWidth), imageHeight_(imageHeight), world_(world),
        cam_(cam), imageBuffer_(imageBuffer),
        pcg32Buffer(imageBuffer.getSize()), spp_(spp) {

    splitIntoTiles(64, 64);
    for (auto &pcg : pcg32Buffer.data_) {
      pcg_extras::seed_seq_from<std::random_device> seed;
      pcg.seed(seed);
    }
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
  Buffer<uint8_t> &imageBuffer_;
  Buffer<pcg32> pcg32Buffer;
  size_t spp_;
  std::vector<Tile> tiles_;

  void splitIntoTiles(size_t tileWidth, size_t tileHeight);
  void renderTiles(Tile &tile, Camera &cam, HittableList &world_);
  Vec3 rayColor(size_t depth, pcg32 &rng, Ray r, Vec3 attenuation);
};
