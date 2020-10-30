cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SAEEngineCore STATIC "source/SAEEngineCore.cpp" "include/SAEEngineCore.h")
target_include_directories(SAEEngineCore PUBLIC "include" PRIVATE "source")
target_link_libraries(SAEEngineCore PRIVATE glfw glad glm)

add_subdirectory("submodules")
add_subdirectory("tests")
