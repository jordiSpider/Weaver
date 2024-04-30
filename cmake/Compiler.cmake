
if(NOT DEFINED RELEASE_MODE AND NOT DEFINED DEBUG_MODE)
	message(FATAL_ERROR "No build type set, 'SelectBuildType.cmake' must be run first.")
endif()


set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")


set(CXX_FLAGS "")
set(CXX_LINK_LIBRARY_FLAGS "")

if(WIN32)
	list(APPEND CXX_FLAGS "-Wall;-fPIC")
	list(APPEND CXX_LINK_LIBRARY_FLAGS "-lm")
elseif(UNIX)
	list(APPEND CXX_FLAGS "-Wall;-fPIC")
	list(APPEND CXX_LINK_LIBRARY_FLAGS "-lm")
else()
	message(FATAL_ERROR "Unknown OS type. Options are: Linux Windows.")
endif()


if(WIN32)
	if(RELEASE_MODE)
		list(APPEND CXX_FLAGS "-O2")
		# Any libraries placed after "-static" will be linked statically
		list(APPEND CXX_LINK_LIBRARY_FLAGS "-static-libgcc;-static-libstdc++;-static;-lwinpthread")
	elseif(DEBUG_MODE)
		list(APPEND CXX_FLAGS "")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	endif()
elseif(UNIX)
	if(RELEASE_MODE)
		list(APPEND CXX_FLAGS "-O2")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	elseif(DEBUG_MODE)
		list(APPEND CXX_FLAGS "-O0;-fno-omit-frame-pointer;-g")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	endif()
else()
	message(FATAL_ERROR "Unknown OS type. Options are: Linux Windows.")
endif()


# ========================================================
# C++ version
#
# I have set version 17 because I use the 'fylesystem'
# library for path management

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ========================================================
# Set output path for executable and libraries

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Build directory" FORCE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# ========================================================
# Set folder and extension file for source code files

set(SRC_DIRECTORY "src")
set(SRC_EXTENSION "cpp")

# ========================================================
# Set folder and extension file for header files

set(INCLUDE_DIRECTORY "include")
set(INCLUDE_DIRECTORY_PATH "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}")
set(INCLUDE_EXTENSION "h")
