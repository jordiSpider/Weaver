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
# 📦 Vcpkg Manifest Feature Configuration
# ------------------------------------------------------------
set(VCPKG_MANIFEST_FEATURES "")

if(BUILD_APP)
    list(APPEND VCPKG_MANIFEST_FEATURES "app")

    if(ENABLE_GUI)
        list(APPEND VCPKG_MANIFEST_FEATURES "gui")
    endif()
endif()


# ------------------------------------------------------------
# Create the vcpkg cache directory if it was injected by the preset
# ------------------------------------------------------------
if(DEFINED ENV{VCPKG_DEFAULT_BINARY_CACHE})
    file(MAKE_DIRECTORY "$ENV{VCPKG_DEFAULT_BINARY_CACHE}")
endif()


# ------------------------------------------------------------
# 🕒 Windows-Compatible Version Generation (MSI Compliant)
# ------------------------------------------------------------
string(TIMESTAMP CURRENT_YEAR "%Y")
string(TIMESTAMP CURRENT_MONTH "%m")
string(TIMESTAMP CURRENT_DAY "%d")

# Convert the year to a relative minor value under 256 (e.g., 2026 -> 26)
math(EXPR MAJOR_VER "${CURRENT_YEAR} - 2000")
# Add 0 to strip leading zeros and prevent CMake from interpreting values as octal
math(EXPR MINOR_VER "${CURRENT_MONTH} + 0")
math(EXPR PATCH_VER "${CURRENT_DAY} + 0")

set(PROGRAM_VERSION "${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}")


# ------------------------------------------------------------
# 🏷️ Define the project
# ------------------------------------------------------------
# The project name is 'Weaver'. The version is defined above,
# and the project uses C++ as its main language.
project(Weaver VERSION ${PROGRAM_VERSION} LANGUAGES CXX)

# Include standard GNU installation directories module
include(GNUInstallDirs)

string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)

# Inject global preprocessor definitions for target binaries
add_compile_definitions(
    DIMENSIONS=2
    ${PROJECT_NAME_UPPER}_PROGRAM_NAME="${PROJECT_NAME}"
    ${PROJECT_NAME_UPPER}_PROGRAM_VERSION="${PROGRAM_VERSION}"
    ${PROJECT_NAME_UPPER}_SCHEMA_VERSION="${SCHEMA_VERSION}"
    ${PROJECT_NAME_UPPER}_SERIALIZATION_VERSION="${SERIALIZATION_VERSION}"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

# ------------------------------------------------------------
# 📂 Subdirectory Targets & External Disclosures
# ------------------------------------------------------------
if(BUILD_APP OR BUILD_TESTS)
    set(CLI_EXE_NAME "${PROJECT_NAME}CLI")
    set(GUI_EXE_NAME "${PROJECT_NAME}GUI")

    add_subdirectory(src)

    # Generate third-party open-source attribution notices
    include(cmake/third_party_notices.cmake)
    generate_third_party_notices(
        "${VCPKG_INSTALLED_DIR}" 
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()
