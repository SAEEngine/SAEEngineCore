cmake_minimum_required (VERSION 3.8)

add_library(SAEEngineCore STATIC "source/SAEEngineCore.cpp" "include/SAEEngineCore.h")
target_include_directories(SAEEngineCore PUBLIC "include" PRIVATE "source")
target_link_libraries(SAEEngineCore 
	PUBLIC 
	SAEEngineCore_Config 
	
	SAEEngineCore_Logging
	SAEEngineCore_FileHandling

	PRIVATE 
	glfw 
	glad 
	glm
)

enable_testing()

add_subdirectory("submodules")
add_subdirectory("tests")
