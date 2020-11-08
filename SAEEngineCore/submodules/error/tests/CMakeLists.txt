
add_executable(SAEEngineCore_Error_Test "error_test.cpp")
target_link_libraries(SAEEngineCore_Error_Test PRIVATE SAEEngineCore)
target_include_directories(SAEEngineCore_Error_Test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")

add_test("SAEEngineCore_Error_Test" SAEEngineCore_Error_Test)
