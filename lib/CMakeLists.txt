
if(TARGET glfw)
else()
	add_subdirectory("glfw")
endif()

if(TARGET SAELib)
else()
	add_subdirectory("saelib")
endif()
