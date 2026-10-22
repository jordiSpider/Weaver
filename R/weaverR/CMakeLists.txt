# =========================================================================
# weaverR R PACKAGE
# =========================================================================
# This directory is only added when at least one of these options is ON
# (see the root CMakeLists.txt):
#
#   BUILD_R_PACKAGES      -> install weaverR in the developer's own R library
#                            (workflow "windows-R-packages")
#   INSTALLER_INCLUDE_R   -> ship the package tarball and dependency metadata in the
#                            installer (workflows "*-pack-*"). Nothing is
#                            installed locally. During installation on the
#                            target machine, compatible system dependencies are
#                            reused and missing/old packages go into Weaver's
#                            standard R library selected for the installer scope.
#
# Targets:
#   weaverR_tarball        flatten + embed schemas + R CMD build   (common)
#   weaverR_install_local  local install            (BUILD_R_PACKAGES)
#   weaverR_payload        installer content        (INSTALLER_INCLUDE_R)
#   weaverR_update_lock    force a new R/renv.lock  (opt-in)
#   weaverR_check          R CMD check              (opt-in)

set(PKG_NAME             weaverR)

find_package(unofficial-noise CONFIG REQUIRED)

add_library(weaver_obstacle_engine SHARED
    "${CMAKE_SOURCE_DIR}/src/App/Model/IBM/Landscape/Map/Patches/ObstaclePorosityMaskGenerator.cpp"
)
set_property(TARGET weaver_obstacle_engine PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_features(weaver_obstacle_engine PRIVATE cxx_std_20)
target_compile_definitions(weaver_obstacle_engine PRIVATE
    WEAVER_OBSTACLE_ENGINE_BUILD
)
target_include_directories(weaver_obstacle_engine PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(weaver_obstacle_engine PRIVATE unofficial::noise::noise-static)
set_target_properties(weaver_obstacle_engine PROPERTIES
    PREFIX ""
    OUTPUT_NAME "weaver_obstacle_engine"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)


# ------------------------------------------------------------------
# Package variant follows the application edition.
# GUI builds include the hub and optimiser; CLI builds are headless.
# ------------------------------------------------------------------
if(ENABLE_GUI)
    set(PKG_VARIANT "full")
else()
    set(PKG_VARIANT "server")
endif()
message(STATUS "[${PKG_NAME}] variant: ${PKG_VARIANT}")

# Dependencies: core is always required, GUI ones only in the full variant
set(_IMPORTS_CORE
    "R6 (>= 2.6.1)"
    "future (>= 1.76.0)"
    "future.apply (>= 1.20.2)"
    "processx (>= 3.8.0)"
    "progressr (>= 1.0.0)"
    "dplyr (>= 1.2.1)"
    "limSolve (>= 2.0.3)"
    "jsonlite (>= 2.0.0)"
    "ggplot2 (>= 4.0.3)"
    "svglite (>= 2.2.2)")
set(_IMPORTS_GUI
    "shiny (>= 1.14.0)"
    "bslib (>= 0.12.0)"
    "shinyFiles (>= 0.9.3)"
    "shinyWidgets (>= 0.9.1)"
    "plotly (>= 4.10.0)"
    "shinyjs"
    "shinyjqui"
    "stats")

if(PKG_VARIANT STREQUAL "full")
    set(_IMPORTS ${_IMPORTS_CORE} ${_IMPORTS_GUI})
    set(PKG_TITLE "Central Tool Hub and Optimiser Framework for Weaver")
    set(PKG_DESCRIPTION "Unified R package wrapping a central 'shiny' tool hub for the
    Weaver simulator and integrating genetic algorithm optimisation modules.")
else()
    set(_IMPORTS ${_IMPORTS_CORE})
    set(PKG_TITLE "Headless Optimiser for Weaver")
    set(PKG_DESCRIPTION "Script interface to the genetic algorithm optimiser of the
    Weaver simulator, without graphical dependencies. Intended for servers.")
endif()
list(JOIN _IMPORTS ",\n    " PKG_IMPORTS)


set(PKG_SRC_DIR          "${CMAKE_CURRENT_SOURCE_DIR}")
set(SCHEMA_SRC_DIR       "${CMAKE_SOURCE_DIR}/schema")
set(FLATTEN_SCRIPT       "${PKG_SRC_DIR}/flatten_package.ps1")
set(EMBED_SCHEMAS_SCRIPT "${PKG_SRC_DIR}/embed_schemas.ps1")
get_filename_component(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../scripts" ABSOLUTE)
get_filename_component(RENV_LOCK   "${CMAKE_CURRENT_SOURCE_DIR}/../renv.lock" ABSOLUTE)
set(CRAN_REPO_FILE "${CMAKE_CURRENT_BINARY_DIR}/cran_repo.txt")
file(WRITE "${CRAN_REPO_FILE}" "${WEAVER_R_REPO}\n")

# PowerShell runs the flattening / schema embedding scripts
find_program(POWERSHELL_EXECUTABLE NAMES powershell pwsh)
if(NOT POWERSHELL_EXECUTABLE)
    message(FATAL_ERROR
        "[${PKG_NAME}] PowerShell not found (needed by flatten_package.ps1). "
        "On Linux install 'pwsh'.")
endif()

# ------------------------------------------------------------------
# Package version = project version, injected in DESCRIPTION.in
# ------------------------------------------------------------------
if(NOT PROGRAM_VERSION)
    message(FATAL_ERROR "[${PKG_NAME}] PROGRAM_VERSION is not defined.")
endif()
set(PKG_VERSION "${PROGRAM_VERSION}")

set(GENERATED_DESCRIPTION "${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION")
configure_file("${PKG_SRC_DIR}/DESCRIPTION.in" "${GENERATED_DESCRIPTION}" @ONLY)

# NAMESPACE = core (+ gui in the full variant)
set(GENERATED_NAMESPACE "${CMAKE_CURRENT_BINARY_DIR}/NAMESPACE")
file(READ "${PKG_SRC_DIR}/NAMESPACE.core" _NS)
if(PKG_VARIANT STREQUAL "full")
    file(READ "${PKG_SRC_DIR}/NAMESPACE.gui" _NS_GUI)
    string(APPEND _NS "\n${_NS_GUI}")
endif()
string(REPLACE "\r\n" "\n" _NS "${_NS}")
# file(CONFIGURE) leaves the file untouched when the content is unchanged
file(CONFIGURE OUTPUT "${GENERATED_NAMESPACE}" CONTENT "${_NS}" @ONLY NEWLINE_STYLE UNIX)

# ------------------------------------------------------------------
# Paths
# ------------------------------------------------------------------
set(STAGE_DIR       "${CMAKE_CURRENT_BINARY_DIR}/staged_package_build")
set(TARBALL         "${CMAKE_CURRENT_BINARY_DIR}/${PKG_NAME}_${PKG_VERSION}.tar.gz")
set(TARBALL_STAMP   "${CMAKE_CURRENT_BINARY_DIR}/tarball.stamp")
set(LOCAL_STAMP     "${CMAKE_CURRENT_BINARY_DIR}/local.stamp")

message(STATUS "[${PKG_NAME}] version ${PKG_VERSION}")
message(STATUS "[${PKG_NAME}] schemas from ${SCHEMA_SRC_DIR}")
if(BUILD_R_PACKAGES)
    message(STATUS "[${PKG_NAME}] mode: install in the local R library")
endif()
if(INSTALLER_INCLUDE_R)
    message(STATUS "[${PKG_NAME}] mode: ship the package in the installer")
endif()

if(NOT EXISTS "${SCHEMA_SRC_DIR}")
    message(FATAL_ERROR "[${PKG_NAME}] Schema directory not found: ${SCHEMA_SRC_DIR}")
endif()

# Files that trigger a rebuild when changed
file(GLOB_RECURSE _PKG_FILES    CONFIGURE_DEPENDS "${PKG_SRC_DIR}/*")
file(GLOB_RECURSE _SCHEMA_FILES CONFIGURE_DEPENDS "${SCHEMA_SRC_DIR}/*")

# ------------------------------------------------------------------
# Common step: flatten + embed schemas + DESCRIPTION + build tarball
# ------------------------------------------------------------------
add_custom_command(
    OUTPUT "${TARBALL_STAMP}"
    # Copy the package to the staging folder, flatten R/ subfolders, and keep target-file copy format unchanged
    COMMAND "${POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass
            -File "${FLATTEN_SCRIPT}" "${PKG_SRC_DIR}" "${STAGE_DIR}" "${PKG_VARIANT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE_DIR}/inst/native"
    COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:weaver_obstacle_engine>" "${STAGE_DIR}/inst/native/"
    # Embed the project schemas: available at runtime via
    # system.file("schema", package = "weaverR")
    COMMAND "${POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass
            -File "${EMBED_SCHEMAS_SCRIPT}" "${SCHEMA_SRC_DIR}" "${STAGE_DIR}"
    # DESCRIPTION generated from DESCRIPTION.in (version = PROGRAM_VERSION)
    COMMAND "${CMAKE_COMMAND}" -E copy "${GENERATED_DESCRIPTION}" "${STAGE_DIR}/DESCRIPTION"
    COMMAND "${CMAKE_COMMAND}" -E copy "${GENERATED_NAMESPACE}"   "${STAGE_DIR}/NAMESPACE"
    # Standard R build (honours .Rbuildignore)
    COMMAND "${CMAKE_COMMAND}" -E remove -f "${TARBALL}"
    COMMAND "${R_EXECUTABLE}" CMD build --no-build-vignettes "${STAGE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${TARBALL_STAMP}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS ${_PKG_FILES} ${_SCHEMA_FILES} "${GENERATED_DESCRIPTION}" "${GENERATED_NAMESPACE}" weaver_obstacle_engine
    COMMENT "[${PKG_NAME}] Flattening, embedding schemas and building the tarball..."
    VERBATIM
)
# Single target owning the command above; the others depend on it with
# add_dependencies() so the tarball is never built twice.
add_custom_target(weaverR_tarball DEPENDS "${TARBALL_STAMP}")

# ------------------------------------------------------------------
# Mode 1: install in the developer's own R library (BUILD_R_PACKAGES)
# ------------------------------------------------------------------
if(BUILD_R_PACKAGES)
    add_custom_command(
        OUTPUT "${LOCAL_STAMP}"
        COMMAND "${RSCRIPT_EXECUTABLE}" "${SCRIPTS_DIR}/install_local.R"
                "${TARBALL}" "${GENERATED_DESCRIPTION}" "${WEAVER_R_REPO}"
        COMMAND "${CMAKE_COMMAND}" -E touch "${LOCAL_STAMP}"
        DEPENDS ${_PKG_FILES} ${_SCHEMA_FILES} "${GENERATED_DESCRIPTION}"
                "${SCRIPTS_DIR}/install_local.R"
        COMMENT "[${PKG_NAME}] Installing in the local R library..."
        VERBATIM
    )
    add_custom_target(weaverR_install_local ALL DEPENDS "${LOCAL_STAMP}")
    add_dependencies(weaverR_install_local weaverR_tarball)
endif()

# ------------------------------------------------------------------
# Mode 2: package shipped in the installer (INSTALLER_INCLUDE_R)
# ------------------------------------------------------------------
if(INSTALLER_INCLUDE_R)
    # The runtime installer uses DESCRIPTION minimum versions and a CRAN
    # snapshot URL; it does not need an exact-version renv.lock restore.
    add_custom_target(weaverR_payload ALL)
    add_dependencies(weaverR_payload weaverR_tarball)

    # Installation rules: component "RPackages"
    if(WIN32)
        # Keep in sync with CM_DP_RPackages.R in the WiX package templates.
        set(R_PAYLOAD_DEST "R")
    else()
        # Keep in sync with WEAVER_R_PAYLOAD_DIR in the root CMakeLists.txt
        set(R_PAYLOAD_DEST "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME_LOWER}/R")
    endif()

    install(FILES
                "${TARBALL}"
                "${CRAN_REPO_FILE}"
                "${SCRIPTS_DIR}/install_r_env.R"
                "${SCRIPTS_DIR}/r_dependency_utils.R"
            DESTINATION "${R_PAYLOAD_DEST}"
            COMPONENT RPackages
    )
    if(WIN32)
        # Silent helper run by the MSI to create the R environment (no console
        # window). Static runtime: it lives in R/, where the VC++ runtime DLLs
        # installed in bin/ would not be found.
        add_executable(WeaverRSetup WIN32
            "${CMAKE_SOURCE_DIR}/cmake/res/Windows/weaver_r_setup.cpp")
        target_compile_definitions(WeaverRSetup PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
        target_link_libraries(WeaverRSetup PRIVATE advapi32 shell32 ole32)
        set_target_properties(WeaverRSetup PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        add_library(WeaverRSetupCA SHARED
            "${CMAKE_SOURCE_DIR}/cmake/res/Windows/weaver_r_setup_ca.cpp")
        target_compile_definitions(WeaverRSetupCA PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
        target_link_libraries(WeaverRSetupCA PRIVATE msi)
        set_target_properties(WeaverRSetupCA PROPERTIES
            PREFIX ""
            OUTPUT_NAME "WeaverRSetupCA"
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        install(TARGETS WeaverRSetup
                RUNTIME DESTINATION "${R_PAYLOAD_DEST}"
                COMPONENT RPackages
        )
    endif()
endif()

# ------------------------------------------------------------------
# Opt-in targets
# ------------------------------------------------------------------
# Force a new lockfile (e.g. to pick up newer dependency versions)
add_custom_target(weaverR_update_lock
    COMMAND "${RSCRIPT_EXECUTABLE}" --vanilla "${SCRIPTS_DIR}/update_lock.R"
            force "${GENERATED_DESCRIPTION}" "${RENV_LOCK}" "${WEAVER_R_REPO}"
    COMMENT "[${PKG_NAME}] Regenerating ${RENV_LOCK} (takes a few minutes)..."
    VERBATIM
)

add_custom_target(weaverR_check
    COMMAND "${CMAKE_COMMAND}" -E env "_R_CHECK_FORCE_SUGGESTS_=false"
            "${R_EXECUTABLE}" CMD check --no-manual "${TARBALL}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "[${PKG_NAME}] Running R CMD check..."
    VERBATIM
)
add_dependencies(weaverR_check weaverR_tarball)
