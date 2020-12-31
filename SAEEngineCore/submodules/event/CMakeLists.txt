cmake_minimum_required (VERSION 3.8)

### Add the name of the submodule, a brief description, the version, and a link to the github repo to the project() call below
###	Example:
###		project(SAEEngineCore_StupidSubmodule VERSION 0.0.1 DESCRIPTION "a very stupid submodule" HOMEPAGE_URL "github.com/StupidSubmodule")
###
### I added names to the fields below to make it easier to use
###
project(  
	SAEEngineCore_Event
	LANGUAGES CXX
	VERSION 0.0.1
	DESCRIPTION "Event handling for the core engine library"
	HOMEPAGE_URL "https://github.com/SAEEngine/SAEEngineCore"
)

### Add the following files to the subdirectories included
###
### include/${PROJECT_NAME}.h
### source/${PROJECT_NAME}.cpp
###

## Create the static library
add_library(${PROJECT_NAME} STATIC "source/${PROJECT_NAME}.cpp" "include/${PROJECT_NAME}.h")

### Add source directories from ./source/* to the command below
### Example:
###
###		set(source_dirs
###			"source/some_source_dir"
###			"source/another_source_dir"
###		)
###
set(source_dirs 
	
)

### Add libary targets to link to below, these will be public
### Example:
###
###		set(link_libs_public
###			SAEEngineCore_Config
###			AnotherStupidLibrary
###		)
###
set(link_libs_public 
	SAEEngineCore_Config
	SAEEngineCore_Widget
	SAELib
)

### Add libary targets to link to below, these will be private
### Example:
###
###		set(link_libs_private
###			SAEEngineCore_Logging
###			glfw
###		)
###
set(link_libs_private
	SAEEngineCore_Logging
)

##
##  End of submodule specific configuration section
##

## Define the source files variable
set(src_files 
	"include/${PROJECT_NAME}Type.h"
)

## Add the source files
target_sources(${PROJECT_NAME} PRIVATE ${src_files})

## Add the source directories
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "${source_dirs}")

## Add the set libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${link_libs_public} PRIVATE ${link_libs_private})

## Set c++ version
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${SAE_ENGINE_CPP_STANDARD} CXX_STANDARD_REQUIRED True)

## Add the module root path to the compile definitions 
target_compile_definitions(${PROJECT_NAME}
	PRIVATE SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}"
	PRIVATE VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
	PRIVATE VERSION_MAJOR="${PROJECT_VERSION_MINOR}"
	PRIVATE VERSION_PATCH="${PROJECT_VERSION_PATCH}"
	PUBLIC ${PROJECT_NAME}_SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}"
	PUBLIC ${PROJECT_NAME}_VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
	PUBLIC ${PROJECT_NAME}_VERSION_MAJOR="${PROJECT_VERSION_MINOR}"
	PUBLIC ${PROJECT_NAME}_VERSION_PATCH="${PROJECT_VERSION_PATCH}"
)

## Add command to generate the event type enumerator
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND "${SAEEngineCore_SOURCE_ROOT}/tools/strenum.exe" ARGS 
	"-i" "${CMAKE_CURRENT_LIST_DIR}/source/event_enum.txt" 
	"-o" "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}Type.h" 
	"-n" "EVENT_TYPE"
	"-s" "sae::engine::core"
	BYPRODUCTS "${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}Type.h")

## Add the source directories
foreach(subdir IN ${source_dirs})
	add_subdirectory(${subdir})
endforeach()

## Enable testing
enable_testing()

## Add tests subdirectory
add_subdirectory("tests")

###
###  Installation handling below
###

if(SAE_ENGINE_CORE_INSTALL)
	install(
		TARGETS ${PROJECT_NAME} 
		EXPORT SAEEngineCore-export
		DESTINATION "lib"
	)
	install(FILES "include/${PROJECT_NAME}.h" DESTINATION "include")
endif()
