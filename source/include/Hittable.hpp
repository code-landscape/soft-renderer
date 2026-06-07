#pragma once

#include "Interval.hpp"
#include "Random.hpp"
#include "Ray.hpp"
#include "Vector.hpp"
#include <cmath>
#include <deque>
#include <limits>
#include <memory>
#include <vector>

class Material;
struct HitInfo {
  Vec3 p, n;
  double t{std::numeric_limits<double>::infinity()};
  bool frontFace;
  Material *mat;
};

class Hittable {
public:
  virtual bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const = 0;
};

class Bbox;
class HittableList : public Hittable {
public:
  HittableList() = default;
  void add(std::shared_ptr<Hittable> object) { objects_.push_back(object); }
  void BVH() {
    HittableList result;
    // result.add(std::make_shared<Bbox>(box, nullptr, nullptr))

    // TODO handle this grossing recursion!!!
    // in split(): pra: a HittableList
    // check sizeof left and right
    // if >3 split it(left or right)
    // get splitted left and right
    // split left and right again
    // then get aabb range(by merging child node)
    // return two HittableList
    // end def
  }
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

class Bbox : public Hittable {
public:
  Bbox(AABB box, std::shared_ptr<HittableList> left,
       std::shared_ptr<HittableList> right)
      : box_(box), right_(right), left_(left) {}

  bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const override {
    double root0;
    double root1;

    {
      root0 = (box_.x_.min_ - r.orig_.x) / r.dir_.x;
      root1 = (box_.x_.max_ - r.orig_.x) / r.dir_.x;
      if (root0 > root1)
        std::swap(root0, root1);
    }
    {
      double t0 = (box_.y_.min_ - r.orig_.y) / r.dir_.y;
      double t1 = (box_.y_.max_ - r.orig_.y) / r.dir_.y;
      if (t0 > t1)
        std::swap(t0, t1);
      root0 = std::max(root0, t0);
      root1 = std::min(root1, t1);
    }
    {
      double t0 = (box_.z_.min_ - r.orig_.z) / r.dir_.z;
      double t1 = (box_.z_.max_ - r.orig_.z) / r.dir_.z;
      if (t0 > t1)
        std::swap(t0, t1);
      root0 = std::max(root0, t0);
      root1 = std::min(root1, t1);
    }
    bool hitAnyThing = root0 < root1;
    if (!hitAnyThing)
      return false;
    hitAnyThing = false;
    if (left_ != nullptr && !hitAnyThing)
      hitAnyThing = left_->hit(r, root0, root1, hitInf);
    if (right_ != nullptr && !hitAnyThing)
      right_->hit(r, tMin, tMax, hitInf);
    return hitAnyThing;
  }

private:
  AABB box_;
  std::shared_ptr<HittableList> left_;
  std::shared_ptr<HittableList> right_;
};

class Sphere final : public Hittable {
public:
  Sphere(Vec3 center, double radius, std::shared_ptr<Material> mat)
      : center_(center), radius_(radius), mat_(mat) {}
  bool hit(Ray &r, double tMin, double tMax, HitInfo &hitInf) const override {
    Vec3 oc = center_ - r.orig_;
    auto a = dot(r.dir_, r.dir_);
    auto h = dot(r.dir_, oc);
    auto c = dot(oc, oc) - radius_ * radius_;
    auto discriminant = h * h - a * c;

    if (discriminant < 0)
      return false;

    auto sqrtd = sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (root < tMin || root > tMax) {
      root = (h + sqrtd) / a;
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
      hitInf.mat = mat_.get();
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
      hitInf.mat = mat_.get();
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
  virtual bool scatter(pcg32 &rng, const Ray &r, const HitInfo &hitInf,
                       Vec3 &attenuation, Ray &scattered) const = 0;
};

class Metal final : public Material {
  bool scatter(pcg32 &rng, const Ray &r, const HitInfo &hitInf,
               Vec3 &attenuation, Ray &scattered) const {
    Vec3 reflected = normalize(reflect(r.dir_, hitInf.n));
    scattered = {hitInf.p, reflected};
    return true;
  }
};

class Lambert final : public Material {
public:
  bool scatter(pcg32 &rng, const Ray &r, const HitInfo &hitInf,
               Vec3 &attenuation, Ray &scattered) const final {
    attenuation = {0.75, 0.75, 0.75};
    scattered = {hitInf.p, hitInf.n + Vec3(sphericalRand(rng))};
    return true;
  }
};

class Dielectric final : public Material {
  bool scatter(pcg32 &rng, const Ray &r, const HitInfo &hitInf,
               Vec3 &attenuation, Ray &scattered) const {
    double ri = hitInf.frontFace ? 1 / 1.5 : 1.5;

    double cosTheta = fmin(1.0f, dot(-normalize(r.dir_), hitInf.n));
    double sinTheta = sqrt(1 - cosTheta * cosTheta);

    if (ri * sinTheta > 1.0 ||
        reflectance(cosTheta, ri) > randomDouble(rng, 0, 1)) {
      Vec3 reflected = normalize(reflect(r.dir_, hitInf.n));
      scattered = {hitInf.p, reflected};
      return true;
    }

    Vec3 refraction = normalize(refract(normalize(r.dir_), hitInf.n, ri));
    scattered = {hitInf.p, refraction};
    return true;
  }

private:
  static double reflectance(double cosine, double refractionIndex) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
};
