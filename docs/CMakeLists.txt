# ============================================================
# 📚 Documentation build configuration (Sphinx + Doxygen)
# ------------------------------------------------------------
# This file configures the documentation build system for the
# Weaver project. It ensures that Sphinx is properly installed
# and meets the minimum version requirements, defines relevant
# documentation paths, and includes the appropriate build
# configuration file depending on the documentation build mode.
# ============================================================


# ------------------------------------------------------------
# ✅ Validate documentation configuration
# ------------------------------------------------------------
set(REQUIRED_DOCS_VARS
	SPHINX_VERSION
	DOCS_DIR_NAME
	SPHINX_BUILD_DIR_NAME
	SPHINX_SOURCE_DIR_NAME
	API_DIR_NAME
	DEFAULT_LANGUAGE
	DOCS_BUILD_TYPE
)

set(MISSING_DOCS_VARS "")
foreach(var_name IN LISTS REQUIRED_DOCS_VARS)
	if(NOT DEFINED ${var_name} OR "${${var_name}}" STREQUAL "")
		list(APPEND MISSING_DOCS_VARS "${var_name}")
	endif()
endforeach()

if(MISSING_DOCS_VARS)
	message(FATAL_ERROR "Missing required documentation settings: ${MISSING_DOCS_VARS}")
endif()


# ------------------------------------------------------------
# 🐍 Locate Python and validate the Sphinx version
# ------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter)

execute_process(
	COMMAND "${Python3_EXECUTABLE}" -m sphinx --version
	RESULT_VARIABLE SPHINX_CHECK_EXIT_CODE
	OUTPUT_VARIABLE SPHINX_VERSION_OUTPUT
	ERROR_VARIABLE SPHINX_VERSION_ERROR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT SPHINX_CHECK_EXIT_CODE EQUAL 0)
	message(FATAL_ERROR
		"Sphinx (v${SPHINX_VERSION} or newer) is required for documentation builds. "
		"Install the documentation packages for ${Python3_EXECUTABLE} with 'python -m pip install sphinx pydata-sphinx-theme sphinx-intl breathe'. "
		"${SPHINX_VERSION_ERROR}"
	)
endif()

execute_process(
	COMMAND "${Python3_EXECUTABLE}" -c "import breathe"
	RESULT_VARIABLE BREATHE_CHECK_EXIT_CODE
	ERROR_VARIABLE BREATHE_CHECK_ERROR
)

if(NOT BREATHE_CHECK_EXIT_CODE EQUAL 0)
	message(FATAL_ERROR
		"Breathe is required for the Weaver API docs. Install it for ${Python3_EXECUTABLE} with 'python -m pip install breathe'. "
		"${BREATHE_CHECK_ERROR}"
	)
endif()

execute_process(
	COMMAND "${Python3_EXECUTABLE}" -c "import pydata_sphinx_theme"
	RESULT_VARIABLE PYDATA_THEME_CHECK_EXIT_CODE
	ERROR_VARIABLE PYDATA_THEME_CHECK_ERROR
)

if(NOT PYDATA_THEME_CHECK_EXIT_CODE EQUAL 0)
	message(FATAL_ERROR
		"The PyData Sphinx theme is required. Install it for ${Python3_EXECUTABLE} with 'python -m pip install pydata-sphinx-theme'. "
		"${PYDATA_THEME_CHECK_ERROR}"
	)
endif()

string(REGEX MATCH "[0-9]+(\\.[0-9]+)+" CURRENT_SPHINX_VERSION "${SPHINX_VERSION_OUTPUT}")
if(NOT CURRENT_SPHINX_VERSION)
	message(FATAL_ERROR "Could not determine the Sphinx version from: ${SPHINX_VERSION_OUTPUT}")
endif()

if(CURRENT_SPHINX_VERSION VERSION_LESS "${SPHINX_VERSION}")
	message(FATAL_ERROR
		"Sphinx ${SPHINX_VERSION} or newer is required, found ${CURRENT_SPHINX_VERSION}."
	)
endif()

message(STATUS "Found Sphinx ${CURRENT_SPHINX_VERSION} using ${Python3_EXECUTABLE}")


# ------------------------------------------------------------
# 📁 Define documentation directory structure
# ------------------------------------------------------------
# The following variables define paths for the documentation
# source, build, and output directories. These paths are derived
# from user-defined variables provided in CMake presets.
set(DOCS_DIR "${PROJECT_SOURCE_DIR}/${DOCS_DIR_NAME}")

set(SPHINX_BUILD_DIR "${DOCS_DIR}/${SPHINX_BUILD_DIR_NAME}")
set(SPHINX_SOURCE_DIR "${DOCS_DIR}/${SPHINX_SOURCE_DIR_NAME}")
set(WEAVER_DOCS_SOURCE_DIR "${SPHINX_SOURCE_DIR}/weaver")

if(NOT DEFINED INCLUDE_DIR OR "${INCLUDE_DIR}" STREQUAL "")
	set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
endif()

if(NOT IS_DIRECTORY "${INCLUDE_DIR}")
	message(FATAL_ERROR "Doxygen input directory does not exist: ${INCLUDE_DIR}")
endif()

if(NOT IS_DIRECTORY "${WEAVER_DOCS_SOURCE_DIR}/latest")
	message(FATAL_ERROR "Weaver Sphinx source directory does not exist: ${WEAVER_DOCS_SOURCE_DIR}/latest")
endif()

# Keep the project sites isolated under a shared GitHub Pages root.
set(DOCS_SITE_OUTPUT_DIR "${SPHINX_BUILD_DIR}/html")
set(SPHINX_DOCS_OUTPUT_DIR "${DOCS_SITE_OUTPUT_DIR}/weaver")
set(MANAGER_DOCS_SOURCE_DIR "${SPHINX_SOURCE_DIR}/weaver-manager/latest")
set(MANAGER_DOCS_OUTPUT_DIR "${DOCS_SITE_OUTPUT_DIR}/weaver-manager")
set(LANDING_DOCS_SOURCE_DIR "${SPHINX_SOURCE_DIR}/landing")

file(GLOB DOCS_WEAVER_RELEASE_DIRS LIST_DIRECTORIES true "${PROJECT_SOURCE_DIR}/manager/releases/*")
set(DOCS_WEAVER_DEFAULT_VERSION "")
set(DOCS_WEAVER_DEFAULT_VERSION_NUMERIC "")
foreach(release_dir IN LISTS DOCS_WEAVER_RELEASE_DIRS)
	if(IS_DIRECTORY "${release_dir}"
		AND EXISTS "${release_dir}/weaver-manager-manifest.json"
		AND EXISTS "${release_dir}/weaver-manager-manifest.json.sig")
		get_filename_component(release_version "${release_dir}" NAME)
		if("${release_version}" MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$"
			AND (DOCS_WEAVER_DEFAULT_VERSION_NUMERIC STREQUAL "" OR "${release_version}" VERSION_GREATER "${DOCS_WEAVER_DEFAULT_VERSION_NUMERIC}"))
			set(DOCS_WEAVER_DEFAULT_VERSION "${release_version}")
			set(DOCS_WEAVER_DEFAULT_VERSION_NUMERIC "${release_version}")
		endif()
	endif()
endforeach()
if(NOT DOCS_WEAVER_DEFAULT_VERSION)
	message(FATAL_ERROR "No Weaver release with both required Manager manifest assets was found under manager/releases.")
endif()
set(DOCS_WEAVER_VERSION "${DOCS_WEAVER_DEFAULT_VERSION}" CACHE STRING "Weaver documentation version to generate")
set(DOCS_WEAVER_CHANNEL "stable" CACHE STRING "Weaver documentation release channel")
set(DOCS_WEAVER_LATEST ON CACHE BOOL "Mark the Weaver documentation version as latest")

if(NOT DEFINED DOCS_MANAGER_VERSION OR "${DOCS_MANAGER_VERSION}" STREQUAL "")
	file(STRINGS "${PROJECT_SOURCE_DIR}/manager/CMakeLists.txt" MANAGER_PROJECT_DECL
		REGEX "^[ \t]*project\\(WeaverManager VERSION ")
	string(REGEX MATCH "VERSION[ \t]+([0-9]+\\.[0-9]+\\.[0-9]+([.-][A-Za-z0-9.-]+)?)"
		MANAGER_PROJECT_VERSION_MATCH "${MANAGER_PROJECT_DECL}")
	if(NOT CMAKE_MATCH_1)
		message(FATAL_ERROR "Could not determine Weaver Manager version from manager/CMakeLists.txt")
	endif()
	set(DOCS_MANAGER_VERSION "${CMAKE_MATCH_1}" CACHE STRING "Weaver Manager documentation version")
endif()

set(DOCS_MANAGER_CHANNEL "stable" CACHE STRING "Weaver Manager documentation release channel")
set(DOCS_MANAGER_LATEST ON CACHE BOOL "Mark the Weaver Manager documentation version as latest")

foreach(product WEAVER MANAGER)
	if(NOT DEFINED DOCS_${product}_VERSION OR "${DOCS_${product}_VERSION}" STREQUAL "")
		message(FATAL_ERROR "DOCS_${product}_VERSION must not be empty")
	endif()
	if(DOCS_${product}_VERSION MATCHES "/" OR DOCS_${product}_VERSION MATCHES "\\\\" OR DOCS_${product}_VERSION STREQUAL "." OR DOCS_${product}_VERSION STREQUAL "..")
		message(FATAL_ERROR "DOCS_${product}_VERSION must be a single path component")
	endif()
	if(NOT DOCS_${product}_CHANNEL STREQUAL "stable" AND NOT DOCS_${product}_CHANNEL STREQUAL "pre-release")
		message(FATAL_ERROR "DOCS_${product}_CHANNEL must be 'stable' or 'pre-release'")
	endif()
	if(DOCS_${product}_CHANNEL STREQUAL "pre-release" AND DOCS_${product}_LATEST)
		message(FATAL_ERROR "A pre-release version cannot be marked latest for ${product}")
	endif()
endforeach()

if(NOT IS_DIRECTORY "${MANAGER_DOCS_SOURCE_DIR}")
	message(FATAL_ERROR "Weaver Manager Sphinx source directory does not exist: ${MANAGER_DOCS_SOURCE_DIR}")
endif()


# ============================================================
# ⚙️ Include documentation build mode configuration
# ============================================================
# Selects and includes the proper build script depending on
# the DOCS_BUILD_TYPE variable defined externally.
# ============================================================

if(DOCS_BUILD_TYPE STREQUAL "Deploy-docs")
	include("${DOCS_DIR}/CMakeDeployDocs.txt")

elseif(DOCS_BUILD_TYPE STREQUAL "Build-docs-dev")
	include("${DOCS_DIR}/CMakeBuildDocsDev.txt")

elseif(DOCS_BUILD_TYPE STREQUAL "Update-docs")
	include("${DOCS_DIR}/CMakeUpdateDocs.txt")

elseif(DOCS_BUILD_TYPE STREQUAL "Build-docs-locale")
	include("${DOCS_DIR}/CMakeBuildDocsLocale.txt")

else()
	message(FATAL_ERROR "Unknown DOCS_BUILD_TYPE: ${DOCS_BUILD_TYPE}")
endif()

if(NOT DOCS_BUILD_TYPE STREQUAL "Build-docs-locale")
	include("${DOCS_DIR}/CMakeBuildManagerDocs.txt")
	include("${DOCS_DIR}/CMakeBuildDocsLanding.txt")
	if(NOT TARGET build-docs)
		add_custom_target(build-docs ALL)
	endif()
	add_dependencies(build-docs build-docs-weaver build-docs-manager build-docs-landing)
endif()
