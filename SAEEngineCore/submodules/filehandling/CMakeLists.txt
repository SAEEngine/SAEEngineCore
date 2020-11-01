
target_sources(SAEEngineCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/FileHandling.h" "${CMAKE_CURRENT_SOURCE_DIR}/FileHandling.cpp")
get_target_property(include_dirs SAEEngineCore INCLUDE_DIRECTORIES)
target_include_directories(SAEEngineCore PRIVATE ${include_dirs} "${CMAKE_CURRENT_LIST_DIR}")

add_subdirectory("tests")