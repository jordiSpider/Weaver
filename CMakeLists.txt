cmake_minimum_required(VERSION 3.25)
project(Weaver VERSION 1.0.0)


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

if(WIN32)
	set(CXX_FLAGS "-Wall")
	set(CXX_LINK_LIBRARY_FLAGS "-lm")
elseif(UNIX)
	set(CXX_FLAGS "-Wall")
	set(CXX_LINK_LIBRARY_FLAGS "-lm")
endif()

if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	set(RELEASE_MODE ON)
	set(DEBUG_MODE OFF)

	if(WIN32)
		list(APPEND CXX_FLAGS "-O0")
		# Any libraries placed after "-static" will be linked statically
		list(APPEND CXX_LINK_LIBRARY_FLAGS "-static-libgcc;-static-libstdc++;-static;-lwinpthread")
	elseif(UNIX)
		list(APPEND CXX_FLAGS "-O0")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	endif()
elseif(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
	set(RELEASE_MODE OFF)
	set(DEBUG_MODE ON)

	if(WIN32)
		list(APPEND CXX_FLAGS "")
		list(APPEND CXX_LINK_LIBRARY_FLAGS "")
	elseif(UNIX)
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

if(USE_CPU_PROFILER)
	list(APPEND CXX_FLAGS "-DUSE_CPU_PROFILER;-lprofiler")
	list(APPEND CXX_LINK_LIBRARY_FLAGS "-lprofiler")
endif()

if(USE_HEAP_PROFILER)
	list(APPEND CXX_FLAGS "-DUSE_HEAP_PROFILER;-ltcmalloc")
	list(APPEND CXX_LINK_LIBRARY_FLAGS "-ltcmalloc")
endif()

set(PATH_SEP "_pathSep_")
set(UNION "_and_")

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
# Set folder for profiler's results

set(PROFILER_DIRECTORY "profiler")

# ========================================================
# Set folder for scripts

set(SCRIPTS_DIRECTORY "scripts")

# ========================================================
# Set folder for schemas

set(SCHEMA_DIRECTORY "schema")

# ========================================================
# Set number of available cores

if(UNIX)
	include(ProcessorCount)
	ProcessorCount(NUMBER_OF_CORES)
elseif(WIN32)
	set(PROCESSOR_COUNT_COMMAND "WMIC CPU Get NumberOfCores")
	execute_process(COMMAND powershell.exe "-Command" "${PROCESSOR_COUNT_COMMAND}"
					OUTPUT_VARIABLE PROCESSOR_COUNT_OUTPUT)
	string(REGEX MATCH "([0-9]+)" NUMBER_OF_CORES_MATCH "${PROCESSOR_COUNT_OUTPUT}")
	set(NUMBER_OF_CORES "${CMAKE_MATCH_1}")
endif()

if (NUMBER_OF_CORES EQUAL 0)
	message(WARNING "Unable to determine number of CPU cores. Setting CMAKE_BUILD_PARALLEL_LEVEL to 4.")
	set(NUMBER_OF_CORES 4)
endif()
message(STATUS "Number of CPU cores: ${NUMBER_OF_CORES}")

set(CMAKE_BUILD_PARALLEL_LEVEL "${NUMBER_OF_CORES}")

# ========================================================
# Set info of the targets

include(cmake/Targets.cmake)

# ========================================================
# Clean files from old preset

# Search all directories within BUILD_FOLDER
file(GLOB DIRS_IN_BUILD_FOLDER "${PROJECT_SOURCE_DIR}/$ENV{BUILD_FOLDER}/*")

# Filter out directories whose name does not match PRESET_NAME
list(FILTER DIRS_IN_BUILD_FOLDER EXCLUDE REGEX "^${PROJECT_SOURCE_DIR}/$ENV{BUILD_FOLDER}/${PRESET_NAME}$")

# Count number of directories obtained
list(LENGTH DIRS_IN_BUILD_FOLDER NUM_DIRECTORIES)

# If directories were found, run the target clean of the found preset and delete its cache.
if(NUM_DIRECTORIES GREATER 0)
	list(GET DIRS_IN_BUILD_FOLDER 0 DIR)
	get_filename_component(PRESET ${DIR} NAME)

	message(STATUS "Cleaning '${PRESET}' preset files")

	# Remove old preset configuration
	file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/$ENV{BUILD_FOLDER}/${PRESET})

	# Remove all libraries
	file(REMOVE_RECURSE ${PROJECT_SOURCE_DIR}/lib)

	# Remove executable
	file(GLOB EXECUTABLES ${PROJECT_SOURCE_DIR}/*${CMAKE_EXECUTABLE_SUFFIX_CXX})
	foreach(EXECUTABLE ${EXECUTABLES})
		file(REMOVE ${EXECUTABLE})
	endforeach()

	message(STATUS "Cleaning '${PRESET}' preset files - done")
endif()

# ========================================================
# Build steps

# ===========================
# Create folder for simulation's results

file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/${RESULT_DIRECTORY}")

if(USE_PROFILER)
	# ===========================
	# Create folder for profiler's results

	file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/${PROFILER_DIRECTORY}")
endif()

# ===========================
# Set library output folder

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# ===========================
# Create folder for libraries

file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# ===========================
# Generate all targets

foreach(target ${TARGETS})
	string(REPLACE "/" ${PATH_SEP} refactorTarget ${target})

	if(target MATCHES ${EXECUTABLE_NAME})
		# ===========================
		# Generate executable

		add_executable(
			${EXECUTABLE_NAME} ${${refactorTarget}_files}
		)
	else()
		# ===========================
		# Generate library

		if(UNIX)
			add_library(${refactorTarget} SHARED ${${refactorTarget}_files})
		elseif(WIN32)
			add_library(${refactorTarget} STATIC ${${refactorTarget}_files})
		endif()
	endif()

	# ===========================
    # Set include directories for target

    target_include_directories(${refactorTarget} PUBLIC "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/")

	# ===========================
    # Set compile flags of target

    target_compile_options(${refactorTarget} PRIVATE ${CXX_FLAGS})

	if(target MATCHES ${EXECUTABLE_NAME})
		# ===========================
		# Set link flags of target

		target_link_options(${refactorTarget} PRIVATE ${CXX_LINK_LIBRARY_FLAGS})
	endif()

	# ===========================
    # Set post build commands of target

    if(target MATCHES "Trait")
		# Read Trait header file content

		file(READ ${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${target}.${INCLUDE_EXTENSION} content)

		# Search for all Trait enum in the content

		string(REGEX MATCHALL ${ENUM_TRAIT_PATTERN} matches "${content}")
		
		# Get the number of matches

		list(LENGTH matches numMatches)

		# If more than one match is found, you will get warned

		if(numMatches GREATER 1)
			message(FATAL_ERROR "Search for traits failed")
		endif()

		# Get the contents of the enumerated

		string(REGEX REPLACE ${ENUM_TRAIT_PATTERN} "\\1" enumTraitValues "${matches}")

		# Remove all white spaces from the content

		string(REGEX REPLACE "[${WHITESPACE}]+" "" enumTraitValues ${enumTraitValues})

		# Add double quotation marks at the beginning and end of each value

		string(REPLACE "," ";" enumTraitValues ${enumTraitValues})
		set(refactorEnumTraitValues)
		foreach(element ${enumTraitValues})
			string(CONCAT refactorElement "\"" ${element} "\"")
			list(APPEND refactorEnumTraitValues ${refactorElement})
		endforeach()
		set(enumTraitValues ${refactorEnumTraitValues})
		string(JOIN "," enumTraitValues ${enumTraitValues})

		# Insert the value of the enumerated in the schema

		configure_file(
			${PROJECT_SOURCE_DIR}/${SCHEMA_DIRECTORY}/species.schema.json.in
			${PROJECT_SOURCE_DIR}/${SCHEMA_DIRECTORY}/species.schema.json
			@ONLY
		)
    endif()
endforeach()

# ===========================
# Link all targets

foreach(target ${TARGETS})
	string(REPLACE "/" ${PATH_SEP} refactorTarget ${target})

	# ===========================
    # Link target with dependencies of the target 

    target_link_libraries(
        ${refactorTarget}
        PUBLIC
        ${${refactorTarget}_depen}
    )
endforeach()

# ========================================================
# Build doxygen documentation

add_subdirectory(docs)
