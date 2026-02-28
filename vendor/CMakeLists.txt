
include(FetchContent)

#glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY	https://github.com/g-truc/glm.git
  GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

#spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.17.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)
