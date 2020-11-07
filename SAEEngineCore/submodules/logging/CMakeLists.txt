
add_library(SAEEngineCore_Logging STATIC "${CMAKE_CURRENT_SOURCE_DIR}/Logging.h" "${CMAKE_CURRENT_SOURCE_DIR}/Logging.cpp")
target_include_directories(SAEEngineCore_Logging PUBLIC "include" PRIVATE "source")
target_link_libraries(SAEEngineCore_Logging PUBLIC SAEEngineCore_Config)

