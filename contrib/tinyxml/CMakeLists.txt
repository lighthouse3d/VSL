add_library(tinyxml STATIC
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp)
	
add_definitions(-DTIXML_USE_STL)	

target_include_directories(tinyxml PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

install (TARGETS tinyxml DESTINATION lib)
install(FILES tinyxml.h DESTINATION include)	
