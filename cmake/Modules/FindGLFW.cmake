# Locate glfw
#
# This module accepts the following environment variables:
#
#    GLFWDIR or GLFW_PATH - Specify the location of GLFW
#
# This module defines the following CMake variables:
#
#    GLFW_FOUND - True if libglfw is found
#    GLFW_LIBRARY - A variable pointing to the GLFW library
#    GLFW_INCLUDE_DIR - Where to find the headers


find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h
  HINTS
	 $ENV{GLFWDIR}
	 $ENV{GLFW_PATH}
  PATH_SUFFIXES
     include
  PATHS
	 ~/Library/Frameworks
	 /Library/Frameworks
	 /usr/local
	 /usr
     /sw # Fink
     /opt/local # DarwinPorts
     /opt/csw # Blastwave
     /opt
)

find_library(GLFW_LIBRARY
  HINTS
	 $ENV{GLFWDIR}
	 $ENV{GLFW_PATH}
  NAMES glfw glfw3
  PATH_SUFFIXES lib
  PATHS
     ~/Library/Frameworks
	 /Library/Frameworks
	 /usr/local
	 /usr
     /sw # Fink
     /opt/local # DarwinPorts
     /opt/csw # Blastwave
     /opt
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)

set(GLFW_LIBRARIES ${GLFW_LIBRARY})
set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
