#pragma once

#include "Vector.hpp"
#include <random>

inline double randomDouble() {
    static thread_local std::mt19937 generator{std::random_device{}()};
    static std::uniform_real_distribution<double> distribution(0, 1);
    return distribution(generator);
}

inline Vec3 sphericalRand(double raidus) {
    // The code below is copied/adapted from GLM to use thread_local random number generator
    double theta = randomDouble() * 6.283185307179586476925286766559;
    double phi = std::acos(randomDouble() * 2.0 - 1.0);

    double x = std::sin(phi) * std::cos(theta);
    double y = std::sin(phi) * std::sin(theta);
    double z = std::cos(phi);

    return Vec3(x, y, z) * raidus;
}
