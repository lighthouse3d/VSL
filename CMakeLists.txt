cmake_minimum_required (VERSION 3.5)

project (vslDemo)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
endif()

if(WIN32) 
	set(CMAKE_DEBUG_POSTFIX "d") 
endif(WIN32)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/bin")

#define settings for VS solution
if (WIN32)
	# add a user file to auto config the working directory for debugging
	set(VP_WORKING_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vsconfig.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/demo/demo.vcxproj.user @ONLY)

	# set the demo project to be the startup project in VS
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
#	get_property(v DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT)
#	message(STATUS "- VS_STARTUP_PROJECT = ${v}" )
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
option(BUILD_SHARED_LIBS "" OFF)

add_subdirectory(contrib/glew)

add_subdirectory(contrib/tinyxml)

option(ASSIMP_INSTALL_PDB "" OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS "" OFF)
option(ASSIMP_BUILD_TESTS "" OFF)
add_subdirectory(contrib/assimp3.3.1)

option( FREEGLUT_BUILD_DEMOS "" OFF)
option(FREEGLUT_BUILD_STATIC_LIBS "" ON)
option(FREEGLUT_BUILD_SHARED_LIBS "" OFF)
option(INSTALL_PDB "" OFF)
add_subdirectory(contrib/freeglut-3.0.0)

if (WIN32)
	set_target_properties (tinyxml PROPERTIES FOLDER contrib)
	set_target_properties (freeglut_static PROPERTIES FOLDER contrib)
	set_target_properties (assimp PROPERTIES FOLDER contrib)
	set_target_properties (glew PROPERTIES FOLDER contrib)
	set_target_properties (uninstall PROPERTIES FOLDER contrib)
	set_target_properties (zlibstatic PROPERTIES FOLDER contrib)	
endif(WIN32)


add_subdirectory(VSL)

add_subdirectory(demo)

set_target_properties(
	demo PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin)
