cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(source_list ${source_list} "source/SAEEngineCore.cpp" "include/SAEEngineCore.h")

add_subdirectory("submodules")

add_library(SAEEngineCore STATIC ${source_list})
target_include_directories(SAEEngineCore PUBLIC "include" PRIVATE "source")

add_subdirectory("tests")
