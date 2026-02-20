
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Ray.hpp"
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

void writeColor(const Vec3 &col, std::ofstream &file) {
  uint8_t r = int(col.r * 255.999);
  uint8_t g = int(col.g * 255.999);
  uint8_t b = int(col.b * 255.999);
  file.write(reinterpret_cast<char *>(&r), 1);
  file.write(reinterpret_cast<char *>(&g), 1);
  file.write(reinterpret_cast<char *>(&b), 1);
}

HittableList world;

Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation) {

  if (depth == 1000)
    return {1, 0, 1};
  depth++;

  Ray scattered;
  HitInfo hitInf {};
  bool hit = world.hit(r, 0.001, std::numeric_limits<double>::max(), hitInf);
  if (hit) {
    if (hitInf.mat->scatter(r, hitInf, attenuation, scattered))
      return attenuation * rayColor(depth, scattered, attenuation);
  } else {
    return {0, 1, 1};
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
  Camera cam(Vec3{0, -20, -20}, 1.6, 0.9, IMAGE_WIDTH, IMAGE_HEIGHT, 0.8,
             Vec3{0, -1, 0}, Vec3{0, 1, 1}, 100);

  world.add(std::make_shared<Sphere>(Vec3{-5.5, -5, 0}, 5,
                                     std::make_shared<Lambert>()));
  world.add(std::make_shared<Sphere>(Vec3{5.5, -5, 0}, 3,
                                     std::make_shared<Lambert>()));

  world.add(std::make_shared<Plane>(Vec3{0, 0, 0}, Vec3{0, -1, 0},
                                    std::make_shared<Metal>()));

  // for loops
  for (size_t y{0}; y != IMAGE_HEIGHT; y++) {
    std::clog << "\r" << y;
    for (size_t x{0}; x != IMAGE_WIDTH; x++) {
      Vec3 attenuation{1, 1, 1};
      writeColor(rayColor(0, cam.getRay(x, y), attenuation), Imagefile);
    }
  }

  Imagefile.close();
  return 0;
}
