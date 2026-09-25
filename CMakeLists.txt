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
string(TOLOWER "${PROJECT_NAME}" PROJECT_NAME_LOWER)

if(MSVC)
    add_compile_options(/MP)
endif()

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


# ------------------------------------------------------------
# 🚀 Resource & Asset Installation Rules
# ------------------------------------------------------------
if(BUILD_APP)

    # Define configuration path layout depending on the platform
    if(WIN32)
        set(CONFIG_DEST_PATH "AppData")
    else()
        set(CONFIG_DEST_PATH "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME_LOWER}")
    endif()

    # Install the configuration directory
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config"
            DESTINATION ${CONFIG_DEST_PATH}
            COMPONENT Unspecified 
    )

    if(WIN32)
        set(OPTIMISER_DEST_PATH "AppData")
    else()
        set(OPTIMISER_DEST_PATH "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME_LOWER}")
    endif()

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/R/optimiser"
            DESTINATION ${OPTIMISER_DEST_PATH}
            COMPONENT Unspecified 
    )

    # Define data path layout depending on the platform
    if(WIN32)
        set(DATA_DEST_PATH "AppData")
    else()
        set(DATA_DEST_PATH "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}")
    endif()

    # Install the legal license file
    install(FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )

    # Install the README documentation file
    install(FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )

    # ------------------------------------------------------------
    # 📦 CPack Distribution Framework
    # ------------------------------------------------------------
    # Package baseline properties
    if(ENABLE_GUI)
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME}-Desktop-Full")
    else()
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME}-Server-CLI")
    endif()
    
    set(CPACK_PACKAGE_VENDOR "${AUTHOR}")
    set(CPACK_PACKAGE_VERSION ${PROGRAM_VERSION})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Weaver Simulator")

    set(CPACK_PACKAGE_HOMEPAGE_URL ${REPO_URL})
    set(CPACK_PACKAGE_CONTACT ${CONTACT_EMAIL})

    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

    # Extract target host processor architecture
    set(ARCH ${CMAKE_SYSTEM_PROCESSOR})

    if(WIN32)
        # Standardized layout filename for Windows distributions
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${PROJECT_VERSION}_Windows_${ARCH}")
    
    elseif(UNIX AND NOT APPLE)
        # On Linux systems, attempt to extract precise distribution details (e.g., "Ubuntu-22.04")
        # by processing the standard /etc/os-release manifest
        set(OS_RELEASE_NAME "Linux")
    
        if(EXISTS "/etc/os-release")
            file(READ "/etc/os-release" OS_RELEASE_CONTENT)
            string(REGEX MATCH "PRETTY_NAME=\"([^\"]+)\"" _ ${OS_RELEASE_CONTENT})
            if(CMAKE_MATCH_1)
                # Replace whitespace occurrences with hyphens (e.g., "Ubuntu 22.04.3 LTS" -> "Ubuntu-22.04.3-LTS")
                string(REPLACE " " "-" OS_RELEASE_NAME "${CMAKE_MATCH_1}") 
            endif()
        endif()

        # Standardized layout filename for Linux distributions
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${PROJECT_VERSION}_${OS_RELEASE_NAME}_${ARCH}")
    endif()

    # Enable component-aware installation patterns
    set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

    # Explicitly append the "Unspecified" fallback component to the registry
    list(APPEND CPACK_COMPONENTS_ALL "Unspecified")
    
    # UI Component metadata strings for Windows installations (NSIS/WIX formats)
    set(CPACK_COMPONENT_CLI_DISPLAY_NAME "Command Line Interface Tools")
    set(CPACK_COMPONENT_GUI_DISPLAY_NAME "Graphical User Interface")

    # Enforce the dependency component as a mandatory, non-visible system requirement
    set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN TRUE)
    set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED TRUE)
    
    # Permit runtime DLL assets from vcpkg to bundle within the target component
    set(CPACK_APP_COMPONENT_INSTALL ON)
    
    # ------------------------------------------------------------
    # Windows Specific CPack Settings (WiX Toolset)
    # ------------------------------------------------------------
    if(WIN32)
        set(CPACK_GENERATOR "WIX")

        # Set the installation directory name
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")

        # Generate per-machine MSI so Windows requests elevation correctly
        set(CPACK_WIX_INSTALL_SCOPE "perMachine")

        # Map system runtime dependencies (msvcp140, etc.) to target component pools
        set(CPACK_SYSTEM_LIBRARIES_ID "Unspecified")

        # --- Settings for updating the PATH and custom UI ---
        # Specify to CPack to compile your additional WiX code files
        set(CPACK_WIX_EXTRA_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/add_path.wxs"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/advanced_paths_ui.wxs"
        )

        # Use custom advanced UI with separate install and application data paths
        set(CPACK_WIX_UI_REF "WixUI_AdvancedPaths")

        # Specify to CPack to apply your patch when generating main.wxs
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/wix_patch.xml.in"
            "${CMAKE_CURRENT_BINARY_DIR}/wix_patch.xml"
            @ONLY
        )

        set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/wix_patch.xml")

        # Enforce multi-component layout mapping within the CPack build generator matrix
        # Pattern structure: "Build Directory" "Project Name" "Component Identifier" "Install Path"
        # Explicitly enforce the processing of the "Unspecified" component layer
        set(CPACK_INSTALL_CMAKE_PROJECTS 
            "${CMAKE_BINARY_DIR}" "${PROJECT_NAME}" "CLI" "/"
            "${CMAKE_BINARY_DIR}" "${PROJECT_NAME}" "GUI" "/"
            "${CMAKE_BINARY_DIR}" "${PROJECT_NAME}" "Unspecified" "/"
        )
    
        # Scan and dynamically populate standard Visual Studio runtime redistribution prerequisites
        include(InstallRequiredSystemLibraries)

        find_program(CPACK_WIX_CANDLE_EXECUTABLE 
            NAMES candle 
            PATHS "$ENV{WIX}bin" 
        )

        find_program(CPACK_WIX_LIGHT_EXECUTABLE 
            NAMES light 
            PATHS "$ENV{WIX}bin" 
        )

        set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/License.rtf")

        # Persistent application upgrade GUID — do not alter once established in production
        set(CPACK_WIX_UPGRADE_GUID "86D6D602-CCC2-4BE3-A83C-4D9D7A61FAE0")

        set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/${PROJECT_NAME_LOWER}.ico")

        # Start Menu Hierarchy Configuration
        # Generates an isolated folder tree: "Your Company -> Weaver" inside the Start Menu
        set(CPACK_WIX_PROGRAM_MENU_FOLDER "Weaver")

        # Start Menu Application Shortcut Mappings
        # Syntax Layout: "TargetExecutableName" "User-Friendly Display Name String"
        # CRITICAL: The initial field value must strictly match the add_executable() target declaration
        set(CPACK_PACKAGE_EXECUTABLES 
            "${PROJECT_NAME}GUI" "${PROJECT_NAME} Simulator"
            "${PROJECT_NAME}CLI" "${PROJECT_NAME} Terminal"
        )

    endif()

    # ------------------------------------------------------------
    # Linux Specific CPack Settings (Debian Packaging)
    # ------------------------------------------------------------
    if(UNIX AND NOT APPLE)
        # Emits production .deb assets alongside fallback compressed .tar.gz files
        set(CPACK_GENERATOR "DEB")

        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Linux/${PROJECT_NAME_LOWER}.desktop.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Linux/${PROJECT_NAME_LOWER}.desktop"
            @ONLY
        )

        # 1. Desktop Entry Installation (Application System Menus)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Linux/${PROJECT_NAME_LOWER}.desktop"
            DESTINATION "share/applications"
            COMPONENT GUI # Deployed exclusively when the visual application environment is present
        )

        # 2. Icon Asset Installation
        # (Requires a valid source scalable vector or 256x256 pixel image target)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/${PROJECT_NAME_LOWER}.svg"
            DESTINATION "share/icons/hicolor/scalable/apps"
            COMPONENT GUI
        )

        # Debian Maintainer Details
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${AUTHOR} <${CONTACT_EMAIL}>")

        # Consolidated Debian asset package identification prefix
        set(CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME_LOWER}")

        set(CPACK_DEBIAN_PACKAGE_LICENSE "MIT")

        # Monolithic Suite Distribution Control (Unified Installer Single Setup)
        # Forcing this flag to OFF instructs CPack to bypass fine-grained COMPONENT splits, 
        # combining the GUI, CLI, and base library contexts into one single .deb payload.
        set(CPACK_DEB_COMPONENT_INSTALL OFF)
    endif()

    # Load CPack Engine (Must remain at the absolute end of the project declaration)
    include(CPack)
endif()