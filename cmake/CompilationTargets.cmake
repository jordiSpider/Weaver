
if(NOT DEFINED COMPILATION_TARGETS)
    message(FATAL_ERROR "The variable 'COMPILATION_TARGETS', which contains a list of targets, must be defined.")
endif()

# ===========================
# Create folder for libraries

file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})


# ========================================================
# Clean files from other build type presets

file(GLOB all_libraries "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*")
file(GLOB debug_libraries "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*${CMAKE_DEBUG_POSTFIX}*")

# Comprueba si la lista de archivos no está vacía
if(all_libraries)
    if(debug_libraries AND RELEASE_MODE)
        set(CLEAN_LIBRARIES ON)
    elseif((NOT debug_libraries) AND DEBUG_MODE)
        set(CLEAN_LIBRARIES ON)
    else()
        set(CLEAN_LIBRARIES OFF)
    endif()
else()
    set(CLEAN_LIBRARIES OFF)
endif()

if(CLEAN_LIBRARIES)
    # Remove libraries
    file(REMOVE_RECURSE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

    # Remove executables
    file(GLOB debug_executables "${PROJECT_SOURCE_DIR}/*${CMAKE_EXECUTABLE_SUFFIX_CXX}")

    foreach(executable ${debug_executables})
        file(REMOVE ${executable})
    endforeach()

    if(RELEASE_MODE)
        message(STATUS "Cleaning debug files - done")
    elseif(DEBUG_MODE)
        message(STATUS "Cleaning release files - done")
    endif()
endif()


# ===========================
# Generate all targets

foreach(target ${COMPILATION_TARGETS})
    if(target IN_LIST EXECUTABLES)
        # ===========================
        # Generate executable

        add_executable(
            ${target} ${${target}_files}
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

    add_dependencies(main_target ${target})

    # ===========================
    # Set include directories for target

    target_include_directories(${target} PUBLIC "${INCLUDE_DIRECTORY_PATH}/" "$ENV{VCPKG_ROOT}/installed/${VCPKG_HOST_TRIPLET}/include/")

    # ===========================
    # Set compile flags of target

    target_compile_options(${target} PRIVATE ${CXX_FLAGS})

    if(target IN_LIST EXECUTABLES)
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

foreach(target ${COMPILATION_TARGETS})
    # ===========================
    # Link target with dependencies of the target 

    target_link_libraries(
        ${target}
        PUBLIC
        ${${target}_depen}
    )
endforeach()
