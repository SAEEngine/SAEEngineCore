###
###  Add any test directories to the set command below following the standard "build_test" test
###

set(test_directories "build_test" "type_test")


###
###	End of configuration section
###

### SUPER TEMPORARY
add_subdirectory("build_test")

# Add the test directories
#foreach(file IN ${test_directories})
#	add_subdirectory("${file}")
#endforeach()

