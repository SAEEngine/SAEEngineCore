cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD ${SAE_ENGINE_CPP_VERSION})
set(CMAKE_CXX_STANDARD_REQUIRED True)

### Add the name of the submodule, a brief description, the version, and a link to the github repo to the project() call below
###	Example:
###		project(SAEEngineCore_StupidSubmodule VERSION 0.0.1 DESCRIPTION "a very stupid submodule" HOMEPAGE_URL "github.com/StupidSubmodule")
###
### I added names to the fields below to make it easier to use
###
project(  
	SAEEngineCore_FileHandling
	VERSION 0.0.1
	DESCRIPTION "File handling submodule for SAEEngineCore"
	HOMEPAGE_URL "https://github.com/SAEEngine/SAEEngineCore"
)

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

set(link_to_libs_public
	SAEEngineCore_Config
	SAEEngineCore_Logging
)
set(link_to_libs_private
	
)




##
##  End of submodule specific configuration section
##

## Define the source files variable
set(src_files 
	"source/${PROJECT_NAME}.cpp"
	"include/${PROJECT_NAME}.h" 
	"config/Config.h"
)

## Create the static library
add_library(${PROJECT_NAME} STATIC "${src_files}")

## Add the source directories
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "config" "${source_dirs}")

## Add the module version to the compile definitions 
add_definitions(
	-D${PROJECT_NAME}_SOURCE_ROOT="${CMAKE_CURRENT_LIST_DIR}"
	-D${PROJECT_NAME}_VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
	-D${PROJECT_NAME}_VERSION_MAJOR="${PROJECT_VERSION_MINOR}"
	-D${PROJECT_NAME}_VERSION_PATCH="${PROJECT_VERSION_PATCH}"
)

## Link to libraries specified
target_link_libraries(${PROJECT_NAME} PUBLIC ${link_to_libs_public} PRIVATE ${link_to_libs_private})

## Set any config variables
set(SUBMODULE_ROOT "${CMAKE_CURRENT_LIST_DIR}")

## Create config file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config/Config.h")

## Add the source directories
foreach(subdir IN ${source_dirs})
	add_subdirectory(${subdir})
endforeach()

## Enable testing
enable_testing()

## Add test directory
add_subdirectory("tests")





