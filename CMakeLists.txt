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
option(INSTALLER_INCLUDE_R "Include the R library in the installer" OFF)

set(VCPKG_MANIFEST_FEATURES "")

if(BUILD_APP OR BUILD_BENCHMARKS)
    list(APPEND VCPKG_MANIFEST_FEATURES "app")
    list(APPEND VCPKG_MANIFEST_FEATURES "obstacle-engine")

    if(ENABLE_GUI)
        list(APPEND VCPKG_MANIFEST_FEATURES "gui")
    endif()
endif()

if(BUILD_BENCHMARKS)
    list(APPEND VCPKG_MANIFEST_FEATURES "benchmarks")
endif()

if(BUILD_R_PACKAGES OR INSTALLER_INCLUDE_R)
    list(APPEND VCPKG_MANIFEST_FEATURES "obstacle-engine")
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
# Keep the numeric project version convenient for CMake/R metadata, while using
# a zero-padded date version for installer names and per-version directories.
set(PROGRAM_VERSION_PADDED "${MAJOR_VER}.${CURRENT_MONTH}.${CURRENT_DAY}")


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

if(ENABLE_GUI)
    set(WEAVER_EDITION "Desktop")
    set(GUI_EXE_NAME "${PROJECT_NAME}GUI")
else()
    set(WEAVER_EDITION "Server")
endif()
string(TOLOWER "${WEAVER_EDITION}" WEAVER_EDITION_LOWER)
set(WEAVER_INSTALLER_ID "${PROJECT_NAME}-${PROGRAM_VERSION_PADDED}-${WEAVER_EDITION}")
set(WEAVER_INSTALL_SUBDIR "${PROGRAM_VERSION_PADDED}/${WEAVER_EDITION}")

# Every package installs into its own version/edition tree under the platform
# package root (Program Files/Weaver on Windows, /opt/Weaver on Linux).
set(CMAKE_INSTALL_BINDIR "${WEAVER_INSTALL_SUBDIR}/bin")
set(CMAKE_INSTALL_SBINDIR "${WEAVER_INSTALL_SUBDIR}/sbin")
set(CMAKE_INSTALL_LIBEXECDIR "${WEAVER_INSTALL_SUBDIR}/libexec")
set(CMAKE_INSTALL_SYSCONFDIR "${WEAVER_INSTALL_SUBDIR}/etc")
set(CMAKE_INSTALL_SHAREDSTATEDIR "${WEAVER_INSTALL_SUBDIR}/com")
set(CMAKE_INSTALL_LOCALSTATEDIR "${WEAVER_INSTALL_SUBDIR}/var")
set(CMAKE_INSTALL_LIBDIR "${WEAVER_INSTALL_SUBDIR}/lib")
set(CMAKE_INSTALL_INCLUDEDIR "${WEAVER_INSTALL_SUBDIR}/include")
set(CMAKE_INSTALL_DATAROOTDIR "${WEAVER_INSTALL_SUBDIR}/share")
set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}")
set(CMAKE_INSTALL_INFODIR "${CMAKE_INSTALL_DATADIR}/info")
set(CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_DATADIR}/man")
set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME}")

if(MSVC)
    add_compile_options(/MP)
endif()

# Inject global preprocessor definitions for target binaries
add_compile_definitions(
    DIMENSIONS=2
    ${PROJECT_NAME_UPPER}_PROGRAM_NAME="${PROJECT_NAME}"
    ${PROJECT_NAME_UPPER}_PROGRAM_VERSION="${PROGRAM_VERSION}"
    ${PROJECT_NAME_UPPER}_PROGRAM_VERSION_DISPLAY="${PROGRAM_VERSION_PADDED}"
    ${PROJECT_NAME_UPPER}_PROGRAM_EDITION="${WEAVER_EDITION}"
    ${PROJECT_NAME_UPPER}_SCHEMA_VERSION="${SCHEMA_VERSION}"
    ${PROJECT_NAME_UPPER}_SERIALIZATION_VERSION="${SERIALIZATION_VERSION}"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

if(BUILD_APP OR BUILD_BENCHMARKS OR BUILD_R_PACKAGES OR INSTALLER_INCLUDE_R)
    include(cmake/weaver_shared_targets.cmake)
endif()

# ------------------------------------------------------------
# 📂 Subdirectory Targets & External Disclosures
# ------------------------------------------------------------
if(BUILD_APP OR BUILD_BENCHMARKS)
    add_subdirectory(src)

    # Generate third-party open-source attribution notices
    include(cmake/third_party_notices.cmake)
    generate_third_party_notices(
        "${VCPKG_INSTALLED_DIR}" 
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()

if(BUILD_APP)
    # 1. Always include the CLI target (serves as the core simulator foundation)
    add_subdirectory(src/cli)

    # 2. Conditionally append the Graphical User Interface (GUI) target layer
    if(ENABLE_GUI)
        add_subdirectory(src/gui)
    endif()
endif()

if(BUILD_APP)
    install(TARGETS weaver_config_validator weaver_obstacle_engine
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Unspecified
        LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Unspecified
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Unspecified
    )
endif()

if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

if(BUILD_R_PACKAGES OR INSTALLER_INCLUDE_R)
    add_subdirectory(R)
endif()


# ------------------------------------------------------------
# 🚀 Resource & Asset Installation Rules
# ------------------------------------------------------------
if(BUILD_APP)
 
    # ------------------------------------------------------------
    # 🚀 Resource & Asset Installation Rules
    # ------------------------------------------------------------
 
    # Define configuration path layout depending on the platform
    if(WIN32)
        # CPack's Directory chooser points at the shared Documents\Weaver root;
        # the version and edition are appended inside the installer.
        set(CONFIG_DEST_PATH "AppData/${PROGRAM_VERSION_PADDED}/${WEAVER_EDITION}")
    else()
        set(CONFIG_DEST_PATH "${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME_LOWER}")
    endif()
 
    # Install the configuration directory (component: Unspecified, always installed)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config"
            DESTINATION ${CONFIG_DEST_PATH}
            COMPONENT Unspecified
    )
 
    # NOTE: the "RPackages" component (weaverR tarball + dependency installer
    # scripts that install into this version and edition's private R library)
    # is installed by R/weaverR/CMakeLists.txt.
 
    # Define data path layout depending on the platform
    if(WIN32)
        set(DATA_DEST_PATH "AppData/${PROGRAM_VERSION_PADDED}/${WEAVER_EDITION}")
    else()
        set(DATA_DEST_PATH "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}")
    endif()
 
    # Install the legal license file
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT Unspecified
    )
 
    # Install the README documentation file
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT Unspecified
    )
 
    # ------------------------------------------------------------
    # 📦 CPack Distribution Framework
    # ------------------------------------------------------------
    # Package baseline properties
    if(ENABLE_GUI)
        set(WEAVER_PACKAGE_VARIANT "Desktop")
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME} ${WEAVER_PACKAGE_VARIANT}")
    else()
        set(WEAVER_PACKAGE_VARIANT "Server")
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME} ${WEAVER_PACKAGE_VARIANT}")
    endif()
 
    set(CPACK_PACKAGE_VENDOR "${AUTHOR}")
    set(CPACK_PACKAGE_VERSION ${PROGRAM_VERSION_PADDED})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Weaver Simulator")
 
    set(CPACK_PACKAGE_HOMEPAGE_URL ${REPO_URL})
    set(CPACK_PACKAGE_CONTACT ${CONTACT_EMAIL})
 
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
 
    # Extract target host processor architecture
    set(ARCH ${CMAKE_SYSTEM_PROCESSOR})
    if(ARCH MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
        set(ARCH "x64")
    elseif(ARCH MATCHES "^(aarch64|AARCH64|arm64|ARM64)$")
        set(ARCH "arm64")
    endif()
 
    if(WIN32)
        # Standardized layout filename for Windows distributions
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WEAVER_PACKAGE_VARIANT}-${PROGRAM_VERSION_PADDED}-Windows-${ARCH}")
 
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
        set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${WEAVER_PACKAGE_VARIANT}-${PROGRAM_VERSION_PADDED}-${OS_RELEASE_NAME}-${ARCH}")
    endif()
 
    # ------------------------------------------------------------
    # 🌳 CPack Components (selectable tree in the installer)
    # ------------------------------------------------------------
    # Component list depends on the build type. "Unspecified" holds the base
    # installer content (config, LICENSE, README, runtime libs) and must
    # always be present. Do NOT override CPACK_COMPONENTS_ALL from package presets.
    set(CPACK_COMPONENTS_ALL Unspecified CLI)
    if(ENABLE_GUI)
        list(APPEND CPACK_COMPONENTS_ALL GUI)
    endif()
    if(INSTALLER_INCLUDE_R)
        list(APPEND CPACK_COMPONENTS_ALL RPackages)
    endif()
 
    # Base component: hidden and mandatory
    set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN   TRUE)
    set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED TRUE)
 
    # CLI: visible but cannot be unchecked
    set(CPACK_COMPONENT_CLI_DISPLAY_NAME "Weaver CLI Tools")
    set(CPACK_COMPONENT_CLI_DESCRIPTION  "Command line interface of the Weaver simulator.")
    set(CPACK_COMPONENT_CLI_REQUIRED     TRUE)
 
    # GUI: optional (only listed when ENABLE_GUI is ON)
    set(CPACK_COMPONENT_GUI_DISPLAY_NAME "Weaver Graphical Interface")
    set(CPACK_COMPONENT_GUI_DESCRIPTION  "Graphical user interface of the Weaver simulator.")
 
    # R packages: the weaverR tarball and installer scripts. weaverR and its
    # dependencies are installed in a private R library for this version/edition.
    # Optional and unchecked by default (remove the DISABLED line to check it by default).
    set(CPACK_COMPONENT_RPACKAGES_DISPLAY_NAME "R packages")

    if(ENABLE_GUI)
        set(_RPKG_DESC "The weaverR package (with a graphical interface).")
    else()
        set(_RPKG_DESC "The weaverR package.")
    endif()
    set(CPACK_COMPONENT_RPACKAGES_DESCRIPTION
        "${_RPKG_DESC} Requires R and internet access to install dependencies not included with R.")

    set(CPACK_COMPONENT_RPACKAGES_DISABLED     FALSE)
 
    # ------------------------------------------------------------
    # Windows Specific CPack Settings (WiX Toolset)
    # ------------------------------------------------------------
    if(WIN32)
        set(CPACK_GENERATOR "WIX")

        add_library(WeaverRSetupCA SHARED
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/weaver_r_setup_ca.cpp")
        target_compile_definitions(WeaverRSetupCA PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
        target_link_libraries(WeaverRSetupCA PRIVATE msi)
        set_target_properties(WeaverRSetupCA PROPERTIES
            PREFIX ""
            OUTPUT_NAME "WeaverRSetupCA"
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        # Keep the helper's logs scoped to this edition, matching the MSI's
        # separate upgrade and uninstall identity.
        if(TARGET WeaverRSetup)
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/weaver_r_setup_config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/weaver_r_setup_config.h"
                @ONLY
            )
            target_include_directories(WeaverRSetup PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
        endif()
        if(TARGET WeaverRSetupCA)
            set(WEAVER_R_SETUP_CA_DLL "$<TARGET_FILE:WeaverRSetupCA>")
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/weaver_r_setup_ca.wxs.in"
                "${CMAKE_CURRENT_BINARY_DIR}/weaver_r_setup_ca.wxs.in"
                @ONLY
            )
            file(GENERATE
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/weaver_r_setup_ca.wxs"
                INPUT "${CMAKE_CURRENT_BINARY_DIR}/weaver_r_setup_ca.wxs.in"
                CONDITION "$<CONFIG:Release>"
            )
        endif()
 
        # Keep a shared package root; the install destinations below add the
        # version and edition directories under it.
        set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
        set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PROJECT_NAME} ${WEAVER_PACKAGE_VARIANT} ${PROGRAM_VERSION_PADDED}")
        string(REPLACE "/" "." _WEAVER_INSTALL_SUBDIR_ID "${WEAVER_INSTALL_SUBDIR}")
        set(WEAVER_CLI_BIN_DIR_ID "CM_DP_CLI.${_WEAVER_INSTALL_SUBDIR_ID}.bin")
        set(WEAVER_R_PAYLOAD_DIR_ID "CM_DP_RPackages.${_WEAVER_INSTALL_SUBDIR_ID}.R")
        set(WEAVER_VERSION_DIR_ID "CM_DP_Unspecified.${PROGRAM_VERSION_PADDED}")

        # Distinct product/upgrade/component identities prevent Windows
        # Installer from treating another version as an upgrade of this one.
        set(_WEAVER_UUID_NAMESPACE "6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        string(UUID CPACK_WIX_PRODUCT_GUID NAMESPACE "${_WEAVER_UUID_NAMESPACE}"
            NAME "${WEAVER_INSTALLER_ID}-product" TYPE SHA1)
        string(UUID CPACK_WIX_UPGRADE_GUID NAMESPACE "${_WEAVER_UUID_NAMESPACE}"
            NAME "${WEAVER_INSTALLER_ID}-upgrade" TYPE SHA1)
        string(UUID WEAVER_SET_PATH_GUID NAMESPACE "${_WEAVER_UUID_NAMESPACE}"
            NAME "${WEAVER_INSTALLER_ID}-path-component" TYPE SHA1)
        string(UUID WEAVER_APPDATA_PATH_GUID NAMESPACE "${_WEAVER_UUID_NAMESPACE}"
            NAME "${WEAVER_INSTALLER_ID}-appdata-component" TYPE SHA1)
        string(UUID WEAVER_REMOVE_R_LOGS_GUID NAMESPACE "${_WEAVER_UUID_NAMESPACE}"
            NAME "${WEAVER_INSTALLER_ID}-r-logs-component" TYPE SHA1)
        string(UUID WEAVER_REMOVE_INSTALL_DIR_GUID NAMESPACE "${_WEAVER_UUID_NAMESPACE}"
            NAME "${WEAVER_INSTALLER_ID}-remove-install-dir-component" TYPE SHA1)
        string(TOUPPER "${CPACK_WIX_PRODUCT_GUID}" CPACK_WIX_PRODUCT_GUID)
        string(TOUPPER "${CPACK_WIX_UPGRADE_GUID}" CPACK_WIX_UPGRADE_GUID)
        string(TOUPPER "${WEAVER_SET_PATH_GUID}" WEAVER_SET_PATH_GUID)
        string(TOUPPER "${WEAVER_APPDATA_PATH_GUID}" WEAVER_APPDATA_PATH_GUID)
        string(TOUPPER "${WEAVER_REMOVE_R_LOGS_GUID}" WEAVER_REMOVE_R_LOGS_GUID)
        string(TOUPPER "${WEAVER_REMOVE_INSTALL_DIR_GUID}" WEAVER_REMOVE_INSTALL_DIR_GUID)

        # Component GUIDs are unique per version and edition, matching their
        # distinct target directories in Windows Installer.
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/r_libs.wxs"
            "${CMAKE_CURRENT_BINARY_DIR}/r_libs.wxs"
            @ONLY
        )
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/add_path.wxs"
            "${CMAKE_CURRENT_BINARY_DIR}/add_path.wxs"
            @ONLY
        )

        # Customize major-upgrade behavior and expose the pre-install notices.
        set(CPACK_WIX_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/main.wxs.in")
 
        # Generate per-machine MSI so Windows requests elevation correctly
        set(CPACK_WIX_INSTALL_SCOPE "perMachine")
 
        # --- Settings for updating the PATH and custom UI ---
        set(CPACK_WIX_EXTRA_SOURCES
            "${CMAKE_CURRENT_BINARY_DIR}/add_path.wxs"
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/advanced_paths_ui.wxs"
            "${CMAKE_CURRENT_BINARY_DIR}/r_libs.wxs"
            "${CMAKE_CURRENT_BINARY_DIR}/weaver_r_setup_ca.wxs"
        )
 
        # WixUtilExtension provides util:RemoveFolderEx for installer-log cleanup.
        # It is needed by candle and light.
        set(CPACK_WIX_EXTENSIONS        "WixUtilExtension")
        set(CPACK_WIX_CANDLE_EXTENSIONS "WixUtilExtension")
 
        # Use custom advanced UI with separate install and application data paths
        set(CPACK_WIX_UI_REF "WixUI_AdvancedPaths")
 
        # Apply the patch when generating main.wxs
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Windows/wix_patch.xml.in"
            "${CMAKE_CURRENT_BINARY_DIR}/wix_patch.xml"
            @ONLY
        )
 
        set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/wix_patch.xml")
 
        # Visual Studio runtime redistributables (installed in the default
        # "Unspecified" component) alongside this version and edition.
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${WEAVER_INSTALL_SUBDIR}/bin")
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
 
        set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/${PROJECT_NAME_LOWER}.ico")
 
        # Start Menu shortcuts. Syntax: "TargetExecutableName" "Display Name"
        # The first field must match the add_executable() target name.
        # The GUI shortcut only exists when the GUI is built.
        set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}CLI" "${PROJECT_NAME} ${PROGRAM_VERSION_PADDED} Terminal")
        if(ENABLE_GUI)
            list(PREPEND CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}GUI" "${PROJECT_NAME} ${PROGRAM_VERSION_PADDED} Simulator")
        endif()
    endif()
 
    # ------------------------------------------------------------
    # Linux Specific CPack Settings (Debian Packaging)
    # ------------------------------------------------------------
    if(UNIX AND NOT APPLE)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${PROJECT_NAME}")

        if(ENABLE_GUI)
            set(WEAVER_DESKTOP_ID "${PROJECT_NAME_LOWER}-${PROGRAM_VERSION_PADDED}-${WEAVER_EDITION_LOWER}")
            set(WEAVER_DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${WEAVER_DESKTOP_ID}.desktop")
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Linux/${PROJECT_NAME_LOWER}.desktop.in"
                "${WEAVER_DESKTOP_FILE}"
                @ONLY
            )
            install(FILES "${WEAVER_DESKTOP_FILE}"
                DESTINATION "/usr/share/applications"
                COMPONENT GUI
            )
            install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/${PROJECT_NAME_LOWER}.svg"
                DESTINATION "/usr/share/icons/hicolor/scalable/apps"
                RENAME "${WEAVER_DESKTOP_ID}.svg"
                COMPONENT GUI
            )
        endif()
 
        # Debian Maintainer Details
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${AUTHOR} <${CONTACT_EMAIL}>")
 
        # Debian package name
        set(CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME_LOWER}-${PROGRAM_VERSION_PADDED}-${WEAVER_EDITION_LOWER}")
        set(CPACK_DEBIAN_PACKAGE_VERSION "${PROGRAM_VERSION_PADDED}")
 
        set(CPACK_DEBIAN_PACKAGE_LICENSE "MIT")
 
        # Single monolithic .deb (GUI, CLI and base content together)
        set(CPACK_DEB_COMPONENT_INSTALL OFF)
 
        # R packages: the .deb ships the weaverR tarball and dependency
        # metadata. The postinst installs the package and dependencies in this
        # version and edition's private library under the installation prefix.
        if(INSTALLER_INCLUDE_R)
            # Keep in sync with R_PAYLOAD_DEST in R/weaverR/CMakeLists.txt
            set(WEAVER_R_PAYLOAD_DIR "${CPACK_PACKAGING_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}/R")
            set(WEAVER_R_LIBRARY_DIR "${CPACK_PACKAGING_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/R")
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Linux/postinst.in"
                "${CMAKE_CURRENT_BINARY_DIR}/debian/postinst"
                @ONLY
            )
            configure_file(
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/res/Linux/postrm.in"
                "${CMAKE_CURRENT_BINARY_DIR}/debian/postrm"
                @ONLY
            )
            set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
                "${CMAKE_CURRENT_BINARY_DIR}/debian/postinst"
                "${CMAKE_CURRENT_BINARY_DIR}/debian/postrm"
            )
            # Force 0755 on the maintainer scripts regardless of the build host
            set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)
            # apt installs R if it is missing
            set(CPACK_DEBIAN_PACKAGE_DEPENDS "r-base-core")
        endif()
    endif()
 
    # Load CPack Engine (must remain at the absolute end of the BUILD_APP block)
    include(CPack)
endif()
