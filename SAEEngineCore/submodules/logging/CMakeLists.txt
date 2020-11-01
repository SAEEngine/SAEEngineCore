
target_sources(SAEEngineCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Logging.h" "${CMAKE_CURRENT_SOURCE_DIR}/Logging.cpp")
get_target_property(include_dirs SAEEngineCore INCLUDE_DIRECTORIES)
target_include_directories(SAEEngineCore PRIVATE ${include_dirs} "${CMAKE_CURRENT_LIST_DIR}")
