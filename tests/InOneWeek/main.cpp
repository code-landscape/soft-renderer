
#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Renderer.hpp"
#include "Vector.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>

HittableList world;

// constants
constexpr size_t IMAGE_WIDTH{3840};
constexpr size_t IMAGE_HEIGHT{2160};
constexpr size_t SPP{1024};

int main(int argc, char *argv[]) {

  std::ofstream Imagefile("Image.ppm", std::ios::binary);
  Imagefile << "P6\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

  // camera
  Camera cam(Vec3{0, -5, -30}, 1.6, 0.9, IMAGE_WIDTH, IMAGE_HEIGHT, 0.8,
             Vec3{0, -1, 0}, Vec3{0, 0, 1}, 10);

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
          -10,
          -4.0001,
          0,
      },
      4, std::make_shared<Metal>()));
  world.add(std::make_shared<Sphere>(
      Vec3{
          -15,
          -3.0001,
          -7,
      },
      3, std::make_shared<Metal>()));
  world.add(std::make_shared<Sphere>(
      Vec3{
          -7,
          -2.0001,
          -14,
      },
      2, std::make_shared<Lambert>()));
  world.add(std::make_shared<Sphere>(
      Vec3{
          10,
          -3.0001,
          0,
      },
      3, std::make_shared<Metal>()));
  world.add(std::make_shared<Sphere>(
      Vec3{
          5,
          -2.0001,
          -10,
      },
      2, std::make_shared<Dielectric>()));
  world.add(std::make_shared<Sphere>(
      Vec3{
          13,
          -1.0001,
          -5,
      },
      1, std::make_shared<Lambert>()));
  world.add(std::make_shared<Plane>(Vec3{0, 0, 0}, Vec3{0, -1, 0},
                                    std::make_shared<Lambert>()));

  // for loops
  Buffer<uint8_t> imageBuffer(IMAGE_WIDTH * IMAGE_HEIGHT * 3);

  CPURenderer renderer(IMAGE_WIDTH, IMAGE_HEIGHT, world, cam, imageBuffer, SPP);

  renderer.render();

  Imagefile.write(reinterpret_cast<const char *>(imageBuffer.getData()),
                  imageBuffer.getSize());
  Imagefile.close();
  return 0;
}
