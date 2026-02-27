
#include "Renderer.hpp"
#include <random>

std::random_device rd;
thread_local std::mt19937_64 rng(rd());
Vec3 rayColor(size_t depth, Ray r, Vec3 attenuation, HittableList &world) {

  if (depth == 1000)
    return {1, 0, 1};
  depth++;

  Ray scattered;
  HitInfo hitInf{};
  bool hit = world.hit(r, 0.0001, std::numeric_limits<double>::max(), hitInf);
  if (hit) {
    if (hitInf.mat->scatter(rng, r, hitInf, attenuation, scattered))
      return attenuation * rayColor(depth, scattered, attenuation, world);
  } else {
    auto a = 0.5 * (normalize(r.dir_).y + 1);
    Vec3 down{0.5, 0.7, 1.0};
    Vec3 up{1, 1, 1.0};
    return a * down + (1 - a) * up;
    return {1, 1, 1};
  }
  return {1, 0, 0};
}
