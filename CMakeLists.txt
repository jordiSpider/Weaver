# ============================================================
# 🧱 CMake configuration file for the Weaver project
# ------------------------------------------------------------
# This file defines the global project configuration,
# versioning, and build entry point. It delegates
# the actual build logic to mode-specific CMake files.
# ============================================================


# ------------------------------------------------------------
# ✅ Minimum CMake version required
# ------------------------------------------------------------
# Ensures that CMake features introduced in version 3.25 or later
# are available. Using an explicit version prevents compatibility
# issues with older CMake installations.
cmake_minimum_required(VERSION 3.25)

# ------------------------------------------------------------
# 🕒 Generate project version from current date
# ------------------------------------------------------------
# The project version is automatically generated from the
# current system date using the format YYYY.MM.DD.
# This allows automatic daily versioning for reproducible builds.
string(TIMESTAMP PROGRAM_VERSION "%Y.%m.%d")


# ------------------------------------------------------------
# 🏷️ Define the project
# ------------------------------------------------------------
# The project name is 'Weaver'. The version is defined above,
# and the project uses C++ as its main language.
project(Weaver VERSION ${PROGRAM_VERSION} LANGUAGES CXX)


# ------------------------------------------------------------
# 📁 Define include directory
# ------------------------------------------------------------
# INCLUDE_DIR_NAME is provided through CMake presets.
# Here we build the absolute path to the include directory.
# Example: ${CMAKE_SOURCE_DIR}/include
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/${INCLUDE_DIR_NAME}")


# ------------------------------------------------------------
# © Generate LICENSE file with current year
# ------------------------------------------------------------
# The LICENSE file is generated automatically from LICENSE.in,
# replacing placeholders such as '@COPYRIGHT_YEAR@' with the
# current year. This ensures the license always reflects the
# current copyright year without manual edits.
# ------------------------------------------------------------
string(TIMESTAMP COPYRIGHT_YEAR "%Y" UTC)
configure_file("${CMAKE_SOURCE_DIR}/LICENSE.in"
               "${CMAKE_SOURCE_DIR}/LICENSE"
               @ONLY)


# ============================================================
# ⚙️ Include build mode configuration
# ============================================================
# Depending on the PROJECT_BUILD_TYPE (set in presets), the
# appropriate build configuration file is included.
# ------------------------------------------------------------
# - Executables → Includes CMakeExecutables.txt
# - Docs        → Includes CMakeDocs.txt
# ------------------------------------------------------------
# Any unrecognized PROJECT_BUILD_TYPE results in a fatal error.
# ============================================================

if(PROJECT_BUILD_TYPE STREQUAL "Executables")
    # Include the configuration file responsible for building executables
    include(${CMAKE_SOURCE_DIR}/CMakeExecutables.txt)

elseif(PROJECT_BUILD_TYPE STREQUAL "Docs")
    # Include the configuration file responsible for building documentation
    include(${CMAKE_SOURCE_DIR}/CMakeDocs.txt)

else()
    # Unknown project type: stop configuration with an error message
    message(FATAL_ERROR "Unknown PROJECT_BUILD_TYPE: ${PROJECT_BUILD_TYPE}")
endif()
