cmake_minimum_required (VERSION 3.5)

project (vslDemo)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()

if(WIN32) 
	set(CMAKE_DEBUG_POSTFIX "d") 
endif(WIN32)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#define settings for VS solution
if (WIN32)
	# add a user file to auto config the working directory for debugging
	set(VP_WORKING_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vsconfig.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/demo/demo.vcxproj.user @ONLY)

	# set the demo project to be the startup project in VS
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
	get_property(v DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT)
	message(STATUS "- VS_STARTUP_PROJECT = ${v}" )
	set (__VSL_TEXTURE_LOADING__ 1)
else()
	find_package(DevIL QUIET)
	if (IL_FOUND)
		set (__VSL_TEXTURE_LOADING__ 1)
	else()
		set (__VSL_TEXTURE_LOADING__ 0)
	endif(IL_FOUND)
endif(WIN32)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OpenGL not found!")
endif(NOT OPENGL_FOUND)

OPTION(INSTALL_PDB "" OFF)

add_subdirectory(contrib/glew)

add_subdirectory(contrib/tinyxml)

option(ASSIMP_BUILD_ASSIMP_TOOLS "" OFF)
option(ASSIMP_BUILD_TESTS "" OFF)
add_subdirectory(contrib/assimp3.3.1)

option( FREEGLUT_BUILD_DEMOS "" OFF)
OPTION(FREEGLUT_BUILD_STATIC_LIBS "" OFF)
add_subdirectory(contrib/freeglut-3.0.0)

add_subdirectory(VSL)

add_subdirectory(demo)

