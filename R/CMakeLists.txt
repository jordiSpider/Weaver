# =========================================================================
# R TOOLCHAIN DISCOVERY
# =========================================================================
# R (the launcher used for "R CMD ...") and Rscript (used to run scripts) are
# located separately: they are different executables and "Rscript CMD INSTALL"
# does not work.

set(_R_HINTS "")

if(WIN32)
    # 1. Official installer registry entry (most reliable)
    cmake_host_system_information(
        RESULT _R_REG_PATH
        QUERY WINDOWS_REGISTRY "HKLM\\SOFTWARE\\R-core\\R"
        VALUE "InstallPath"
        VIEW 64
        ERROR_VARIABLE _R_REG_ERROR
    )
    if(NOT _R_REG_ERROR AND _R_REG_PATH)
        file(TO_CMAKE_PATH "${_R_REG_PATH}" _R_REG_PATH)
        list(APPEND _R_HINTS "${_R_REG_PATH}/bin/x64" "${_R_REG_PATH}/bin")
    endif()

    # 2. Fallback: scan standard folders, newest R version first
    foreach(_BASE_PATH IN ITEMS "C:/Program Files/R" "C:/Program Files (x86)/R")
        if(EXISTS "${_BASE_PATH}")
            file(GLOB _R_VERSIONS "${_BASE_PATH}/R-*")
            list(SORT _R_VERSIONS COMPARE NATURAL ORDER DESCENDING)
            foreach(_R_VERSION IN LISTS _R_VERSIONS)
                list(APPEND _R_HINTS "${_R_VERSION}/bin/x64" "${_R_VERSION}/bin")
            endforeach()
        endif()
    endforeach()
endif()

find_program(R_EXECUTABLE      NAMES R      HINTS ${_R_HINTS})
find_program(RSCRIPT_EXECUTABLE NAMES Rscript HINTS ${_R_HINTS})
mark_as_advanced(R_EXECUTABLE RSCRIPT_EXECUTABLE)

if(NOT R_EXECUTABLE OR NOT RSCRIPT_EXECUTABLE)
    message(FATAL_ERROR
        "R was not found on this system.\n"
        "Install R (independently of RStudio) or pass the paths explicitly:\n"
        "  -DR_EXECUTABLE=\"C:/Program Files/R/R-4.x.y/bin/x64/R.exe\"\n"
        "  -DRSCRIPT_EXECUTABLE=\"C:/Program Files/R/R-4.x.y/bin/x64/Rscript.exe\""
    )
endif()

# Report the R version. The bundled R library contains compiled packages, so the
# R version used here must match the major.minor series (e.g. 4.5) of the R
# installed on the machines where the installer will be used.
execute_process(
    COMMAND "${RSCRIPT_EXECUTABLE}" --vanilla -e "cat(as.character(getRversion()))"
    OUTPUT_VARIABLE R_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "R ${R_VERSION_STRING} found: ${R_EXECUTABLE}")

# CRAN repository used to resolve the package dependencies.
# A dated Posit Package Manager snapshot makes builds reproducible.
set(WEAVER_R_REPO "https://packagemanager.posit.co/cran/2026-10-01"
    CACHE STRING "CRAN repository (or dated snapshot) used to resolve R dependencies")

add_subdirectory(weaverR)