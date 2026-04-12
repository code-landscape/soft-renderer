
#SDL3
find_package(SDL3 REQUIRED)

#imgui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

file(GLOB imgui_HEADERS ${imgui_SOURCE_DIR}/*h)
file(GLOB imgui_SOURCES ${imgui_SOURCE_DIR}/*cpp)


file(
  GLOB
  imgui_BACKENDS
  ${imgui_SOURCE_DIR}backends/imgui_impl_sdl3.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

add_library(
  imgui
  STATIC

  ${imgui_HEADERS}
  ${imgui_SOURCES}
  ${imgui_BACKENDS}
)

target_include_directories(
  imgui
  PUBLIC
  ${imgui_SOURCE_DIR}
  ${GLFW_SOURCE_DIR}/include
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
  imgui
  ${SDL3_LIBRARIES}
)

project(
  InOneWeek
  CXX
)

add_executable(
  InOneWeek
  ./InOneWeek/main.cpp
)

target_link_libraries(
  InOneWeek
  RayChasing
  imgui
)

project(
  imgui_viewer
  CXX
)

add_executable(
  imgui_viewer
  ./imgui_viewer/main.cpp
)

target_link_libraries(
  imgui_viewer
  RayChasing
  imgui
)
