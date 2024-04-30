cmake_minimum_required(VERSION 3.25)
project(Weaver VERSION 1.0.0)


set(WINDOWS FALSE)
set(LINUX FALSE)
set(DARWIN FALSE)
if(OS_NAME STREQUAL "Windows")
	set(WINDOWS TRUE)
elseif(OS_NAME STREQUAL "Linux")
	set(LINUX TRUE)
elseif(OS_NAME STREQUAL "Darwin")
	set(DARWIN TRUE)
endif()


if(NOT CMAKE_BUILD_TYPE)
	set(
		CMAKE_BUILD_TYPE "Release" CACHE STRING
      	"Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      	FORCE
	)
endif()

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_DEBUG "")

if(WINDOWS)
	set(CXX_FLAGS "-Wall")
	set(CXX_LINK_LIBRARY_FLAGS "-lm")
elseif(LINUX)
	set(CXX_FLAGS "-Wall")
	set(CXX_LINK_LIBRARY_FLAGS "-lm")
elseif(DARWIN)
	set(CXX_FLAGS "-Wall")
	set(CXX_LINK_LIBRARY_FLAGS "-lm")
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	set(RELEASE_MODE ON)
	set(DEBUG_MODE OFF)

	if(WINDOWS)
		list(APPEND CXX_FLAGS "-O2")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "-static-libgcc;-static-libstdc++;-lwinpthread")
	elseif(LINUX)
		list(APPEND CXX_FLAGS "-O2")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	elseif(DARWIN)
		list(APPEND CXX_FLAGS "-O2")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	endif()
elseif(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
	set(RELEASE_MODE OFF)
	set(DEBUG_MODE ON)

	if(WINDOWS)
		list(APPEND CXX_FLAGS "")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	elseif(LINUX)
		list(APPEND CXX_FLAGS "-O0;-fno-omit-frame-pointer;-g")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	elseif(DARWIN)
		list(APPEND CXX_FLAGS "-O0;-fno-omit-frame-pointer;-g")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	endif()
else()
	message(FATAL_ERROR "Unknown Cmake build type. Options are: Debug Release RelWithDebInfo MinSizeRel.")
endif()

if(CHECK_MEMORY_LEAKS)
	list(APPEND CXX_FLAGS "-fsanitize=address")
	list(APPEND CXX_LINK_LIBRARY_FLAGS "-fsanitize=address")
endif()


# ========================================================
# Dependencies

find_package(
	Python3 3.7.0 
	COMPONENTS Interpreter
)

if(NOT Python3_FOUND)
	message(FATAL_ERROR "Python3 (3.7.0 or more) need to be installed to build the project")
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
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib CACHE PATH "Build directory" FORCE)

# ========================================================
# Set folder and extension file for source code files

set(SRC_DIRECTORY "src")
set(SRC_EXTENSION "cpp")

# ========================================================
# Set folder and extension file for header files

set(INCLUDE_DIRECTORY "include")
set(INCLUDE_EXTENSION "h")

# ========================================================
# Set folder for simulation's results

set(RESULT_DIRECTORY "result_simulation")

# ========================================================
# Set main file of the project

set(MAIN_FILENAME "main")

# ========================================================
# Set source code files which aren't classes

set(IGNORED_FILES
    ${MAIN_FILENAME}
)

# ========================================================
# Set folder for scripts

set(SCRIPTS_DIRECTORY "scripts")

# ========================================================
# Set scripts to use in the build

set(LIST_DIRECTORY_SCRIPT "${SCRIPTS_DIRECTORY}/build/listDirectory.py")
set(LIST_LINK_LIBRARIES_SCRIPT "${SCRIPTS_DIRECTORY}/build/listLinkLibraries.py")
set(REFACTOR_CLASS_SCRIPT "${SCRIPTS_DIRECTORY}/build/refactorClass.py")

# ========================================================
# Build steps

# ===========================
# Create folder for simulation's results

file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/${RESULT_DIRECTORY}")

# ===========================
# List the classes' names
#
# The name of a class is equal to the path of the class relative to the SRC_DIRECTORY folder

execute_process(
    COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${LIST_DIRECTORY_SCRIPT}" "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}" "${IGNORED_FILES}"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
    OUTPUT_VARIABLE CLASSES_NAMES
)

set(CLASSES_NAMES_CMAKE_PATH)
foreach(class ${CLASSES_NAMES})
	file(TO_CMAKE_PATH ${class} class)
	list(APPEND CLASSES_NAMES_CMAKE_PATH ${class})
endforeach()
set(CLASSES_NAMES ${CLASSES_NAMES_CMAKE_PATH})

# ===========================
# Build subdirectory lib

add_subdirectory(lib)

# ===========================
# Generate executable

if(RELEASE_MODE)
	set(EXECUTABLE_NAME ${PROJECT_NAME})
elseif(DEBUG_MODE)
	set(EXECUTABLE_NAME ${PROJECT_NAME}_debug)
endif()

add_executable(
	${EXECUTABLE_NAME} ${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${MAIN_FILENAME}.${SRC_EXTENSION}
)

# ===========================
# Set include directories for executable

target_include_directories(${EXECUTABLE_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/")

# ===========================
# List link libraries of executable

execute_process(
	COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${LIST_LINK_LIBRARIES_SCRIPT}" "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${MAIN_FILENAME}.${SRC_EXTENSION}" "${MAIN_FILENAME}" "${CLASSES_NAMES_CMAKE_PATH}"
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
	OUTPUT_VARIABLE LINK_LIBRARIES
)

# ===========================
# Link libraries with executable

target_link_libraries(
	${EXECUTABLE_NAME}
	PRIVATE
	${LINK_LIBRARIES}
)

# ===========================
# Set compile and link flags of executable

target_compile_options(${EXECUTABLE_NAME} PRIVATE ${CXX_FLAGS})
target_link_options(${EXECUTABLE_NAME} PRIVATE ${CXX_LINK_LIBRARY_FLAGS})

# ========================================================
# Build doxygen documentation

add_subdirectory(docs/doxygen)
