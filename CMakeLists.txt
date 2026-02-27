
cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(RayChasing CXX)

add_subdirectory(vendor)

#tbb
find_package(TBB REQUIRED)

add_library(
  RayChasing
  ./source/include/Camera.hpp
  ./source/include/Hittable.hpp
  ./source/include/Ray.hpp
  ./source/include/Vector.hpp
  ./source/include/Renderer.hpp
  ./source/src/Renderer.cpp
)

target_link_libraries(
  RayChasing
  PUBLIC
  glm::glm
  TBB::tbb
)

target_include_directories(
  RayChasing
  PUBLIC
  ${PROJECT_SOURCE_DIR}/source/include
)

add_subdirectory(tests)
