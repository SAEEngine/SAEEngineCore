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
	project_name_goes_here
	VERSION version_goes_here
	DESCRIPTION description_goes_here
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
add_library(${PROJECT_NAME} STATIC ${src_files})

## Add the source directories
target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE "config" ${source_dirs})





## Set any config variables
set(SUBMODULE_ROOT "${CMAKE_CURRENT_LIST_DIR}")

## Create config file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config/Config.h")

## Add the source directories
foreach(subdir IN ${source_dirs})
	add_subdirectory(${subdir})
endforeach()



