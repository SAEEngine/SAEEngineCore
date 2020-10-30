
add_executable(SAEEngineCore_Environment_Test "window_test.cpp")
target_link_libraries(SAEEngineCore_Environment_Test PRIVATE SAEEngineCore)
target_include_directories(SAEEngineCore_Environment_Test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")

add_test("SAEEngineCore_Environment_Test" SAEEngineCore_Environment_Test)
