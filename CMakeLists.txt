cmake_minimum_required (VERSION 3.8)

option(SAE_ENGINE_CORE_USE_EXCEPTIONS "enabled use of exceptions" ON)
option(SAE_ENGINE_CORE_HARD_ERRORS "when ON causes hard aborts when an error is found" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-DPROJECT_ROOT="${CMAKE_CURRENT_LIST_DIR}")

project ("SAEEngineCore" VERSION 0.0.1)

configure_file("SAEEngineCoreConfig.h.in" "SAEEngineCoreConfig.h")

add_subdirectory("lib")
add_subdirectory("SAEEngineCore")