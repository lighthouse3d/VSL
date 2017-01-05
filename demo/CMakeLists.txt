add_executable(demo 
	source/vslDemo.cpp)
	
LINK_DIRECTORIES(${Assimp_BINARY_DIR})
	
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
	
	
if  (WIN32)
	add_definitions(-DGLEW_BUILD)
	file(COPY  ${CMAKE_CURRENT_SOURCE_DIR}/../contrib/devil/IL DESTINATION ${CMAKE_CMAKE_BINARY_DIR}/include)	
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../contrib/devil/lib64/DevIL.dll 
			DESTINATION ${CMAKE_BINARY_DIR}/bin)
		target_link_libraries(demo "../../contrib/devil/lib64/DevIL")
	else()
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../contrib/devil/lib32/DevIL.dll 
			DESTINATION ${CMAKE_BINARY_DIR}/bin)
		target_link_libraries(demo "../../contrib/devil/lib32/DevIL")
	endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
else()
	if (IL_FOUND)
		target_link_libraries(demo ${IL_LIBRARIES} )
	endif(NOT IL_FOUND)
endif(WIN32)

install (TARGETS demo DESTINATION bin)


