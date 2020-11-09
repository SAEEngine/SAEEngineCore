cmake_minimum_required (VERSION 3.8)

set(SAE_ENGINE_CPP_STANDARD 20)

set(CMAKE_CXX_STANDARD ${SAE_ENGINE_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(SAE_ENGINE_CORE_USE_EXCEPTIONS "enabled use of exceptions" OFF)
option(SAE_ENGINE_CORE_INSTALL "allows SAE_ENGINE_CORE to generate install files" ON)

project ("SAEEngineCore" LANGUAGES CXX C VERSION 0.0.1)
include(CTest)

add_library(${PROJECT_NAME}_Config INTERFACE)
if(SAE_ENGINE_CORE_USE_EXCEPTIONS)
	target_compile_definitions(${PROJECT_NAME}_Config 
		INTERFACE SAE_ENGINE_CORE_USE_EXCPETIONS=${SAE_ENGINE_CORE_USE_EXCPETIONS}
	)
endif()

add_subdirectory("lib")
add_subdirectory("SAEEngineCore")

enable_testing()
