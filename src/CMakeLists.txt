
if(DEBUG_MODE)
    list(APPEND CXX_FLAGS "-DDEBUG")
endif()

list(APPEND CXX_FLAGS "-DPROJECT_VERSION=\"${PROJECT_VERSION}\"")

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


set(TARGETS_PATHS "")

if(BUILD_IBM)
    list(APPEND TARGETS_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/IBM/${PROJECT_NAME}.${SRC_EXTENSION}")
endif()

if(BUILD_UPDATE_CONFIG_TOOL)
    list(APPEND TARGETS_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Tools/updateConfig.${SRC_EXTENSION}")
endif()

if(BUILD_OPTIMISER_TOOL)
    list(APPEND TARGETS_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Tools/optimiser.${SRC_EXTENSION}")
endif()


include("${PROJECT_SOURCE_DIR}/cmake/GenerateTargets.cmake")


if(USE_CPU_PROFILER)
    list(APPEND ${WEAVER_EXECUTABLE}_depen profiler)
endif()

if(USE_HEAP_PROFILER)
    list(APPEND ${WEAVER_EXECUTABLE}_depen tcmalloc)
endif()


include("${PROJECT_SOURCE_DIR}/cmake/CompilationTargets.cmake")


if(BUILD_IBM)
    # ========================================================
    # Set folder for simulation's results

    set(RESULT_DIRECTORY "result_simulation")

    # ===========================
    # Create folder for simulation's results

    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/${RESULT_DIRECTORY}")
endif()

if(USE_CPU_PROFILER OR USE_HEAP_PROFILER)
    # ========================================================
    # Set folder for profiler's results

    set(PROFILER_DIRECTORY "profiler")

    # ===========================
    # Create folder for profiler's results

    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/${PROFILER_DIRECTORY}")
endif()
