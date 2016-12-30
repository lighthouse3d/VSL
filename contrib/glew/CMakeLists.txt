add_library(glew STATIC
	glew.c)
	
target_include_directories(glew PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")	
target_link_libraries(glew ${OPENGL_LIBRARIES}  )
add_definitions(-DGLEW_BUILD)

install (TARGETS glew DESTINATION lib)
install(DIRECTORY GL DESTINATION include)
