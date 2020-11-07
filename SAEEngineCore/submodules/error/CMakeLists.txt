
target_sources(SAEEngineCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Error.h" "${CMAKE_CURRENT_SOURCE_DIR}/Error.cpp")
target_include_directories(SAEEngineCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

enable_testing()
add_subdirectory("tests")