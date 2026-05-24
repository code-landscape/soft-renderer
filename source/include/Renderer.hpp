
#pragma once

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Ray.hpp"
#include "pcg/pcg_random.hpp"
#include <cstddef>
#include <tbb/parallel_for.h>

static inline uint8_t toByte(double col) { return int(col * 255.999); }

class CPURenderer {
public:
  struct Pixel final {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  struct Tile final {
    size_t fromX_, fromY_, width_, height_;
  };

  CPURenderer(size_t imageWidth, size_t imageHeight, HittableList &world,
              Camera &cam, Buffer<Pixel> &imageBuffer, size_t spp);

  bool render();

private:
  size_t imageWidth_;
  size_t imageHeight_;
  HittableList &world_;
  Camera &cam_;
  Buffer<Pixel> &imageBuffer_;
  Buffer<pcg32> pcg32Buffer;
  size_t spp_;
  std::vector<Tile> tiles_;

  void splitIntoTiles(size_t tileWidth, size_t tileHeight);
  void renderTiles(Tile &tile, Camera &cam, HittableList &world_);
  Vec3 rayColor(size_t depth, pcg32 &rng, Ray r, Vec3 attenuation);
};
