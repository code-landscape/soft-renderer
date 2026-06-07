
cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(RayChasing CXX)

include(FetchContent)

# glm
find_package(glm QUIET)
if(NOT glm_FOUND)
  message(STATUS "glm not found via system, fetching via FetchContent...")
  FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(glm)
endif()

#  spdlog
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
  message(STATUS "spdlog not found via system, fetching via FetchContent...")
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(spdlog)
endif()

# tbb
find_package(TBB REQUIRED)

add_library(
  RayChasing
  ./source/include/Camera.hpp
  ./source/include/Hittable.hpp
  ./source/include/Ray.hpp
  ./source/include/Vector.hpp
  ./source/include/Renderer.hpp
  ./source/include/Interval.hpp
  ./source/src/Renderer.cpp
  ./source/include/pcg/pcg_extras.hpp
  ./source/include/pcg/pcg_uint128.hpp
  ./source/include/pcg/pcg_random.hpp
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
