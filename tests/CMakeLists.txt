
#glfw
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glfw)

#vulkan
find_package(Vulkan REQUIRED)

#imgui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

file(GLOB imgui_HEADERS ${imgui_SOURCE_DIR}/*h)
file(GLOB imgui_SOURCES ${imgui_SOURCE_DIR}/*cpp)


file(
  GLOB
  imgui_BACKENDS
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3

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

project(
  InOneWeek
  CXX
)

add_executable(
  InOneWeek
  ./InOneWeek/main.cpp
)

find_package(TBB QUIET)
if (TBB_FOUND)
  target_compile_definitions(InOneWeek PRIVATE HAS_TBB)
  target_link_libraries(
    InOneWeek
    TBB::tbb
  )
endif()

target_link_libraries(
  InOneWeek
  RayChasing
  imgui
)
