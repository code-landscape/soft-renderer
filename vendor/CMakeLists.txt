
include(FetchContent)

# --- glm ---
find_package(glm QUIET)
if(NOT glm_FOUND)
  message(STATUS "glm not found, fetching via FetchContent...")
  FetchContent_Declare(
    glm
    GIT_REPOSITORY	https://github.com/g-truc/glm.git
    GIT_TAG 	1.0.3
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(glm)
endif()

# --- spdlog ---
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
  message(STATUS "spdlog not found, fetching via FetchContent...")
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(spdlog)
endif()
