
#pragma once

struct Interval {
  double min_, max_;
};

struct AABB {
  AABB(Interval x, Interval y, Interval z) : x_(x), y_(y), z_(z) {}
  Interval x_, y_, z_;
  AABB merge() {}
};
