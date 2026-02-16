
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Ray.hpp"
#include "Vector.hpp"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
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

Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation, HitInfo &hitInf) {

  std::cout << "depth: " << depth << std::endl;
  if (depth == 1000)
    return {1, 0, 0};
  depth++;

  Ray scattered;
  bool hit = world.hit(r, 0.001, 100000000, hitInf);
  std::cout << "if hit: " << std::boolalpha << hit << std::endl;
  if (hit) {
    std::cout << "ray orig: " << r.orig_.x << ' ' << r.orig_.y << ' '
              << r.orig_.z << '\n'
              << "ray dir: " << r.dir_.x << ' ' << r.dir_.y << ' ' << r.dir_.z
              << '\n';
    std::cout << "hit normal: " << hitInf.n.x << ' ' << hitInf.n.y << ' '
              << hitInf.n.z << std::endl;
    std::cout << "hit point: " << hitInf.p.x << ' ' << hitInf.p.y << ' '
              << hitInf.p.z << std::endl;
    std::cout << "hit root: " << hitInf.t << std::endl;
    if (hitInf.mat->scatter(r, hitInf, attenuation, scattered)) {
      std::cout << "scattered orig: " << scattered.orig_.x << ' '
                << scattered.orig_.y << ' ' << scattered.orig_.z << '\n'
                << "scattered dir: " << scattered.dir_.x << ' '
                << scattered.dir_.y << ' ' << scattered.dir_.z << '\n';
      return attenuation * rayColor(depth, scattered, attenuation, hitInf);
    }
  } else {
    return {0, 1, 1};
  }
  std::cout << "scatter failed" << std::endl;
  return {1, 0, 1};
}

// constants
constexpr size_t IMAGE_WIDTH{192 * 5};
constexpr size_t IMAGE_HEIGHT{108 * 5};
constexpr size_t SPP{31};

int main(int argc, char *argv[]) {

  std::ofstream Imagefile("Image.ppm", std::ios::binary);
  Imagefile << "P6\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

  // camera
  Camera cam(Vec3{0, -1, -7}, 1.6, 0.9, IMAGE_WIDTH, IMAGE_HEIGHT, 0.8,
             Vec3{0, -1, 0}, Vec3{0, 0, 1}, 100);

  world.add(std::make_shared<Sphere>(Vec3{0, 100, 0}, 100,
                                     std::make_shared<Lambert>()));
  world.add(
      std::make_shared<Sphere>(Vec3{0, -1, 0}, 1, std::make_shared<Metal>()));

  // world.add(std::make_shared<Plane>(Vec3{0, 0, 0}, Vec3{0, -1, 0},
  //                                   std::make_shared<Metal>()));

  // for loops
  for (size_t y{0}; y != IMAGE_HEIGHT; y++) {
    for (size_t x{0}; x != IMAGE_WIDTH; x++) {
      std::cout << "\nx: " << x << " y: " << y << std::endl;
      Vec3 attenuation{1, 1, 1};
      HitInfo hitInf;
      writeColor(rayColor(0, cam.getRay(x, y), attenuation, hitInf), Imagefile);
    }
  }

  Imagefile.close();
  return 0;
}
