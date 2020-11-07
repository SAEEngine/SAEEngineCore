###
###	Jonathan Cline - 11/7/2020
###

## DO NOT RENAME THE "test.cpp" FILE INCLUDED IN THIS FOLDER

### Adds a new test executable 'test_exe' linked to library 'for_library'.
###  Example :  
###		define_test(simple_test SAEEngineCore)
###		this would produce a new test executable named test linked to library SAEEngineCore
macro(define_test test_exe, for_library)
	add_executable(${ARGV0} "test.cpp")
	target_link_libraries(${ARGV0} PRIVATE ${ARGV2})
endmacro(define_test)

### Creates an instance of the test 'test_exe' named 'test_name'. Command line arguements can be passed by adding them
###	  as additional parameters
###  Example :  
###		new_test_instance("simple_test_base" simple_test)
###	 Example with command arguements :
###		new_test_instance("simple_test_2" simple_test 2 19 "a string of sorts")
macro(new_test_instance test_name, test_exe)
	add_test(NAME "${ARGV0}" COMMAND "${ARGV1}" ${ARVN})
endmacro(new_test_instance)

### Example of defining a new test and creating two instances of it
###
###	(directory structure)
###		./CMakeLists.txt
###		./test.cpp
###
### define_test(WindowOpenTest SAEEngineCore_Window)
### new_test_instance("window_open_test_fullscreen" WindowOpenTest "fullscreen")
### new_test_instance("window_open_test_windowed" WindowOpenTest "windowed" 600 400)
###
