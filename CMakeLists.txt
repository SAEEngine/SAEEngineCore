cmake_minimum_required (VERSION 3.8)

set(SAE_ENGINE_CPP_STANDARD 20)

set(CMAKE_CXX_STANDARD ${SAE_ENGINE_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(SAE_ENGINE_CORE_USE_EXCEPTIONS "enabled use of exceptions" ON)
option(SAE_ENGINE_CORE_HARD_ERRORS "enables hard aborts when an error is found" ON)
option(SAE_ENGINE_CORE_INSTALL "allows SAE_ENGINE_CORE to generate install files" ON)

project ("SAEEngineCore" LANGUAGES CXX C VERSION 0.0.1)
include(CTest)

configure_file("${CMAKE_CURRENT_LIST_DIR}/config/${PROJECT_NAME}_Config.h.in" "${CMAKE_CURRENT_LIST_DIR}/config/${PROJECT_NAME}_Config.h")

add_library(${PROJECT_NAME}_Config INTERFACE)
target_include_directories(${PROJECT_NAME}_Config INTERFACE "config")

add_subdirectory("lib")
add_subdirectory("SAEEngineCore")

enable_testing()
