
#include "Renderer.hpp"
#include <algorithm>
#include <cstddef>
#include <pcg/pcg_random.hpp>

void CPURenderer::splitIntoTiles(size_t tileWidth, size_t tileHeight) {
  tiles_.clear();
  for (size_t y = 0; y < imageHeight_; y += tileHeight) {
    for (size_t x = 0; x < imageWidth_; x += tileWidth) {
      size_t w = std::min(tileWidth, imageWidth_ - x);
      size_t h = std::min(tileHeight, imageHeight_ - y);
      tiles_.emplace_back(x, y, w, h);
    }
  }
}

void CPURenderer::renderTiles(Tile &tile, Camera &cam, HittableList &world_) {
  for (size_t y = tile.fromY_; y < tile.fromY_ + tile.height_; ++y) {
    for (size_t x = tile.fromX_; x < tile.fromX_ + tile.width_; ++x) {
      Vec3 col{0.0, 0.0, 0.0};
      for (size_t s = 0; s < spp_; ++s) {
        Ray r = cam.getRay(x, y);
        auto &seed = pcg32Buffer[y * imageWidth_ + x];
        col += rayColor(0, seed, r, Vec3{1.0, 1.0, 1.0});
        seed.discard(1);
      }
      col /= static_cast<double>(spp_);

      // Write into RGB buffer (row-major, 3 bytes per pixel)
      size_t index = (y * imageWidth_ + x) * 3;
      imageBuffer_[index + 0] = toByte(col.x);
      imageBuffer_[index + 1] = toByte(col.y);
      imageBuffer_[index + 2] = toByte(col.z);
    }
  }
}

Vec3 CPURenderer::rayColor(size_t depth, pcg32 &rng, Ray r, Vec3 attenuation) {

  if (depth == 1000)
    return {1, 0, 1};
  depth++;

  Ray scattered;
  HitInfo hitInf{};
  bool hit = world_.hit(r, 0.0001, std::numeric_limits<double>::max(), hitInf);
  if (hit) {
    if (hitInf.mat->scatter(rng, r, hitInf, attenuation, scattered))
      return attenuation * rayColor(depth, rng, scattered, attenuation);
  } else {
    auto a = 0.5 * (normalize(r.dir_).y + 1);
    Vec3 down{0.5, 0.7, 1.0};
    Vec3 up{1, 1, 1.0};
    return a * down + (1 - a) * up;
    return {1, 1, 1};
  }
  return {1, 0, 0};
}
