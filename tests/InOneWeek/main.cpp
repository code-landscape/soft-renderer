
#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Ray.hpp"
#include "Renderer.hpp"
#include "Vector.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

inline Ray getRay(Camera &cam, size_t x, size_t y) {
  return Ray{cam.orig_, cam.higherLeftCorner + static_cast<double>(x) * cam.u +
                            static_cast<double>(y) * cam.v};
}

// uint8_t toByte(double col) { return int(col * 255.999); }

HittableList world;

Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation) {

  if (depth == 1000)
    return {1, 0, 1};
  depth++;

  Ray scattered;
  HitInfo hitInf{};
  bool hit = world.hit(r, 0.0001, std::numeric_limits<double>::max(), hitInf);
  if (hit) {
    if (hitInf.mat->scatter(r, hitInf, attenuation, scattered))
      return attenuation * rayColor(depth, scattered, attenuation);
  } else {
    auto a = 0.5 * (normalize(r.dir_).y + 1);
    Vec3 up{0.5, 0.7, 1.0};
    Vec3 down{1, 1, 1.0};
    return a * down + (1 - a) * up;
    return {1, 1, 1};
  }
  return {1, 0, 0};
}

// constants
constexpr size_t IMAGE_WIDTH{1920};
constexpr size_t IMAGE_HEIGHT{1080};
constexpr size_t SPP{31};

int main(int argc, char *argv[]) {

  std::ofstream Imagefile("Image.ppm", std::ios::binary);
  Imagefile << "P6\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

  // camera
  Camera cam(Vec3{0, -5, -30}, 1.6, 0.9, IMAGE_WIDTH, IMAGE_HEIGHT, 0.8,
             Vec3{0, -1, 0}, Vec3{0, 0, 1}, 100);

  world.add(std::make_shared<Sphere>(
      Vec3{
          0,
          -5.0001,
          0,
      },
      5, std::make_shared<Lambert>()));

  world.add(std::make_shared<Sphere>(
      Vec3{
          0,
          -3.0001,
          -15,
      },
      3, std::make_shared<Dielectric>()));

  world.add(std::make_shared<Sphere>(
      Vec3{
          -9,
          -4.0001,
          0,
      },
      4, std::make_shared<Metal>()));

  world.add(std::make_shared<Plane>(Vec3{0, 0, 0}, Vec3{0, -1, 0},
                                    std::make_shared<Lambert>()));

  // for loops
  RGBBuffer<IMAGE_WIDTH, IMAGE_HEIGHT> imageBuffer;
  CPURenderer<IMAGE_WIDTH, IMAGE_HEIGHT> renderer(world, cam, imageBuffer);
  renderer.render();
  Imagefile.write(reinterpret_cast<const char *>(imageBuffer.getData()),
                  imageBuffer.getSize());
  Imagefile.close();
  return 0;
}
