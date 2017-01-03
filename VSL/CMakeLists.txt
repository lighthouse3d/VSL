file(GLOB vslSource
	"source/*.cpp")
	
file(GLOB vslHeaders
	"include/vsl/*.h")	

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/vslConfig.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/vsl/vslConfig.h")	
	
add_library(vsl  STATIC
	${vslSource} ${vslHeaders})
	
source_group("Header Files" FILES ${vslHeaders})

target_include_directories(vsl 
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/vsl")

include_directories(
	../contrib/devil/
	../contrib/assimp3.3.1/include
	../contrib/glew
	../contrib/tinyxml)
	
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DTIXML_USE_STL)

install (TARGETS vsl DESTINATION lib)
install (DIRECTORY include/vsl DESTINATION include)