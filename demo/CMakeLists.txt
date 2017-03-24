add_executable(demo 
	source/vslDemo.cpp source/config.h)
	
link_directories(${Assimp_BINARY_DIR})
	
target_link_libraries(demo vsl tinyxml freeglut_static assimp glew)
target_link_libraries(demo ${OPENGL_LIBRARIES} )

include_directories(
	../VSL/include
	../contrib/freeglut-3.0.0/include
	../contrib/assimp3.3.1/include
	../contrib/devil/
	../contrib/tinyxml
	../contrib/glew
	${OpenGL_INCLUDE_DIRS})
	
add_definitions(-DTIXML_USE_STL -DFREEGLUT_STATIC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(PATH_TO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../")

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/config.h"
  )	
	
if  (WIN32)
	add_definitions(-DGLEW_BUILD)
	file(COPY  ${CMAKE_SOURCE_DIR}/contrib/devil/IL DESTINATION ${CMAKE_CMAKE_BINARY_DIR}/include)	
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		file(COPY ${CMAKE_SOURCE_DIR}/contrib/devil/lib64/DevIL.dll 
			DESTINATION ${CMAKE_BINARY_DIR}/bin)
		target_link_libraries(demo "${CMAKE_SOURCE_DIR}/contrib/devil/lib64/DevIL.lib")
	else()
		file(COPY ${CMAKE_SOURCE_DIR}/contrib/devil/lib32/DevIL.dll 
			DESTINATION ${CMAKE_BINARY_DIR}/bin)
		target_link_libraries(demo "${CMAKE_SOURCE_DIR}/contrib/devil/lib32/DevIL.lib")
	endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
else()
	if (IL_FOUND)
		target_link_libraries(demo ${IL_LIBRARIES} )
	endif(NOT IL_FOUND)
endif(WIN32)

install (TARGETS demo DESTINATION bin)


