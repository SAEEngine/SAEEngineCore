
if(TARGET glad)
else()
	add_subdirectory("glad")
endif()

if(TARGET glfw)
else()
	add_subdirectory("glfw")
endif()

if(TARGET glm)
else()
	add_subdirectory("glm")
endif()

if(TARGET SAELib)
else()
	add_subdirectory("saelib")
endif()