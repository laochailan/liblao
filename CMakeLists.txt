cmake_minimum_required(VERSION 2.8)
project(lao)

option(BUILD_DOCUMENTATION "Use Doxygen to create the Documentation" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

add_definitions(-Wall -pedantic -Werror-implicit-function-declaration)

include_directories(include)
add_subdirectory(src)
add_subdirectory(testapp)

if(BUILD_DOCUMENTATION)
	find_package(Doxygen REQUIRED)

	configure_file(docs/Doxyfile.in
		${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
	add_custom_target(Docs ALL COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
endif()
