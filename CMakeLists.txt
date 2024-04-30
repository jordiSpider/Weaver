cmake_minimum_required(VERSION 3.25)
project(Weaver VERSION 23.11.05)


# ========================================================
# Set folder and extension file for header files

set(INCLUDE_DIRECTORY "include")
set(INCLUDE_DIRECTORY_PATH "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}")
set(INCLUDE_EXTENSION "h")


# ========================================================
# Build executable

if(BUILD_EXE)
	if(NOT CMAKE_BUILD_TYPE)
		message(FATAL_ERROR "Choose the type of build, options are: Debug or Release.")
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
			list(APPEND CXX_FLAGS "-O2")
			# Any libraries placed after "-static" will be linked statically
			list(APPEND CXX_LINK_LIBRARY_FLAGS "-static-libgcc;-static-libstdc++;-static;-lwinpthread")
		elseif(UNIX)
			list(APPEND CXX_FLAGS "-O2")
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

	if(DEBUG_MODE)
		list(APPEND CXX_FLAGS "-DDEBUG")
	endif()

	set(PATH_SEP "_pathSep_")
	set(UNION "_")

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
		foreach(PRESET_DIR ${DIRS_IN_BUILD_FOLDER})
			get_filename_component(PRESET ${PRESET_DIR} NAME)

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
		endforeach()
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
		if(target MATCHES ${EXECUTABLE_NAME})
			# ===========================
			# Generate executable

			add_executable(
				${EXECUTABLE_NAME} ${${target}_files}
			)
		else()
			# ===========================
			# Generate library

			if(UNIX)
				add_library(${target} SHARED ${${target}_files})
			elseif(WIN32)
				add_library(${target} STATIC ${${target}_files})
			endif()
		endif()

		# ===========================
		# Set include directories for target

		target_include_directories(${target} PUBLIC "${INCLUDE_DIRECTORY_PATH}/" "$ENV{VCPKG_ROOT}/installed/${VCPKG_HOST_TRIPLET}/include/" ${Boost_INCLUDE_DIRS})

		# ===========================
		# Set compile flags of target

		target_compile_options(${target} PRIVATE ${CXX_FLAGS})

		if(target MATCHES ${EXECUTABLE_NAME})
			# ===========================
			# Set link flags of target

			target_link_options(${target} PRIVATE ${CXX_LINK_LIBRARY_FLAGS})
		endif()

		if(UNIX)
			target_compile_definitions(${target} PRIVATE DIMENSIONS=${DIMENSIONS} UNIX)
		elseif(WIN32)
			target_compile_definitions(${target} PRIVATE DIMENSIONS=${DIMENSIONS} WINDOWS)
		endif()
	endforeach()

	# ===========================
	# Link all targets

	foreach(target ${TARGETS})
		# ===========================
		# Link target with dependencies of the target 

		target_link_libraries(
			${target}
			PUBLIC
			${${target}_depen}
		)
	endforeach()
endif()


# ========================================================
# Build documentation

add_subdirectory(docs)
