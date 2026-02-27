
#pragma once

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include <cstddef>
#include <tbb/parallel_for.h>

Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation, HittableList &world);

inline uint8_t toByte(double col) { return int(col * 255.999); }

template <size_t imageWidthT, size_t imageHeightT> class CPURenderer {
public:
  CPURenderer(HittableList &world, Camera &cam,
              RGBBuffer<imageWidthT, imageHeightT> &imageBuffer, size_t spp)
      : world_(world), cam_(cam), imageBuffer_(imageBuffer), spp_(spp) {}
  bool render() {

    splitIntoTiles(64, 64, 1);

    tbb::parallel_for(size_t(0), tiles_.size(), [this](size_t i) {
      renderTile(tiles_[i], cam_, world_);
    });
    return true;
  };

private:
  HittableList &world_;
  Camera &cam_;
  RGBBuffer<imageWidthT, imageHeightT> &imageBuffer_;
  size_t spp_;
  std::vector<Tile> tiles_;

  void splitIntoTiles(size_t tileWidth, size_t tileHeight, size_t spp) {
    size_t numTilesX = ceil(static_cast<double>(imageWidthT) / tileWidth);
    size_t numTilesY = ceil(static_cast<double>(imageHeightT) / tileHeight);
    for (size_t tileY{0}; tileY != numTilesY; tileY++) {
      for (size_t tileX{0}; tileX != numTilesX; tileX++) {
        size_t fromX = tileX * tileWidth;
        size_t fromY = tileY * tileHeight;
        size_t width = std::min(tileWidth, imageWidthT - fromX);
        size_t height = std::min(tileHeight, imageHeightT - fromY);
        tiles_.emplace_back(fromX, fromY, width, height);
      }
    }
  }
  void renderTile(Tile &tile, Camera &cam, HittableList &world_) {
    for (size_t y{tile.fromY_}; y != tile.fromY_ + tile.height_; y++) {
      for (size_t x{tile.fromX_}; x != tile.fromX_ + tile.width_; x++) {
        Vec3 color{0, 0, 0};
        for (size_t s{0}; s != spp_; s++) {
          Vec3 attenuation{1, 1, 1};
          color += rayColor(0, cam_.getRay(x, y), attenuation, world_);
        }
        color /= static_cast<double>(spp_);
        size_t index = (y * imageWidthT + x) * 3;
        imageBuffer_[index + 0] = toByte(color.r);
        imageBuffer_[index + 1] = toByte(color.g);
        imageBuffer_[index + 2] = toByte(color.b);
      }
    }
  }
};
