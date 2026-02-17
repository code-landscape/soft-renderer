#pragma once

#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <memory>
#include <vector>

class Material;
class HitInfo {
public:
  Vec3 p, n;
  double t{std::numeric_limits<double>::infinity()};
  bool frontFace;
  std::shared_ptr<Material> mat;
};

class Hittable {
public:
  virtual bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const = 0;
};

class HittableList : public Hittable {
public:
  HittableList() = default;
  void add(std::shared_ptr<Hittable> object) { objects_.push_back(object); }
  bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const {
    bool hitAnyThing{false};
    for (auto &obj : objects_) {
      if (obj->hit(r, tMin, tMax, hitInf))
        hitAnyThing = true;
    }
    return hitAnyThing;
  }

private:
  std::vector<std::shared_ptr<Hittable>> objects_;
};

class Sphere final : public Hittable {
public:
  Sphere(Vec3 center, double radius, std::shared_ptr<Material> mat)
      : center_(center), radius_(radius), mat_(mat) {}
  bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const override {
    auto a = dot(r.dir_, r.dir_);
    auto halfB = dot(r.dir_, (r.orig_ - center_));
    auto c = dot((r.orig_ - center_), (r.orig_ - center_)) - radius_ * radius_;
    auto discrimination = halfB * halfB - a * c;

    if (discrimination < 0)
      return false;

    auto sqrtd = sqrt(discrimination);

    auto root = (-halfB - sqrtd) / a;
    if (root < tMin || root > tMax) {
      root = (-halfB + sqrtd) / a;
      if (root < tMin || root > tMax)
        return false;
    }

    if (root < hitInf.t) {
      hitInf.t = root;
      hitInf.p = r.at(hitInf.t);
      hitInf.n = (hitInf.p - center_) / radius_;
      hitInf.frontFace = dot(r.dir_, hitInf.n) < 0;
      if (!hitInf.frontFace)
        hitInf.n = -hitInf.n;
      hitInf.mat = mat_;
    }
    return true;
  }

private:
  Vec3 center_;
  double radius_;
  std::shared_ptr<Material> mat_;
};

class Plane final : public Hittable {
public:
  Plane(Vec3 center, Vec3 normal, std::shared_ptr<Material> mat)
      : center_(center), normal_(normalize(normal)), mat_(mat) {}
  bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const override {
    auto root = dot(center_ - r.orig_, normal_) / dot(r.dir_, normal_);
    if (root < tMin || root > tMax) {
      return false;
    }
    if (root < hitInf.t) {
      hitInf.t = root;
      hitInf.p = r.at(hitInf.t);
      hitInf.n = normal_;
      hitInf.frontFace = dot(r.dir_, hitInf.n) < 0;
      if (!hitInf.frontFace)
        hitInf.n = -hitInf.n;
      hitInf.mat = mat_;
    }
    return true;
  }

private:
  Vec3 center_;
  Vec3 normal_;
  std::shared_ptr<Material> mat_;
};

class Material {
public:
  virtual bool scatter(const Ray &rIn, const HitInfo &hitInf, Vec3 &attenuation,
                       Ray &scattered) const = 0;
};

class Metal final : public Material {
  bool scatter(const Ray &rIn, const HitInfo &hitInf, Vec3 &attenuation,
               Ray &scattered) const {
    Vec3 reflected = normalize(reflect(rIn.dir_, hitInf.n));
    scattered = {hitInf.p + 0.05 * reflected, reflected};
    return true;
  }
};

class Lambert final : public Material {
public:
  bool scatter(const Ray &rIn, const HitInfo &hitInf, Vec3 &attenuation,
               Ray &scattered) const final {
    attenuation = {0.75, 0.75, 0.75};
    scattered = {hitInf.p + 0.001 * hitInf.n,
                 hitInf.n + Vec3(sphericalRand(1.0f))};
    return true;
  }
};
