include(cmake/Regex.cmake)

include(cmake/ExternalLibraries.cmake)


# ============================
# Set main file of the project

set(MAIN_FILENAME "main")

# ===================
# Set executable name

if(RELEASE_MODE)
	set(EXECUTABLE_NAME ${PROJECT_NAME})
elseif(DEBUG_MODE)
	set(EXECUTABLE_NAME ${PROJECT_NAME}_debug)
endif()


# =================================
# Add executable target information

# Add executable to the list of targets
set(TARGETS ${EXECUTABLE_NAME})

# Set main file object library information
set(MAIN_FILENAME_OBJ_LIB "${MAIN_FILENAME}_o")

# Add main file object library to executable target libraries
set(${EXECUTABLE_NAME}_object_libraries ${MAIN_FILENAME_OBJ_LIB})

# Add main file object library to the list of object libraries
set(SIMPLE_OBJECT_LIBRARIES ${MAIN_FILENAME_OBJ_LIB})

# Set the path to the main file object library files
set(OBJECT_LIBRARIES_PATH_${MAIN_FILENAME_OBJ_LIB} ${MAIN_FILENAME})


# ==========================================
# Add remaining targets and object libraries

# List all items in the source directory
file(GLOB targets_paths "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/*")

# Remove the main file from the list
list(REMOVE_ITEM targets_paths "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${MAIN_FILENAME}.${SRC_EXTENSION}")

# Iterate on target paths
while(targets_paths)
    # Extract a path
    list(GET targets_paths 0 elem_path)
    list(REMOVE_AT targets_paths 0)

    # Path management depending on file type
    if(IS_DIRECTORY "${elem_path}")
        # Add directory items to target paths
        file(GLOB listed_items "${elem_path}/*")
        list(APPEND targets_paths ${listed_items})
    else()
        # Extract the name of the target to which the library belongs
        string(REGEX MATCH ${TARGET_PATH} matchedPart "${elem_path}")
        set(target ${CMAKE_MATCH_1})
        string(REPLACE "/" ${UNION} refactorTarget ${target})

        # Add new target to the list of targets
        list(APPEND TARGETS ${refactorTarget})

        # Extract the object library path
        string(REGEX MATCH ${CLASS_PATH} matchedPart "${elem_path}")
        set(classPath ${CMAKE_MATCH_1})

        # Extract the object library name
        string(REGEX MATCH ${CLASS_NAME} matchedPart "${classPath}")
        set(class ${CMAKE_MATCH_0})
        set(OBJ_LIB "${class}_o")

        # Check duplicates object libraries
        if(${OBJ_LIB} IN_LIST SIMPLE_OBJECT_LIBRARIES)
            message(FATAL_ERROR "Duplicate object library found: ${OBJ_LIB}")
        endif()

        # Add new object library to the list of object libraries
        list(APPEND SIMPLE_OBJECT_LIBRARIES ${OBJ_LIB})

        # Set the path to the new object library files
        set(OBJECT_LIBRARIES_PATH_${OBJ_LIB} ${classPath})

        # Add the new object library to the target library list.
        list(APPEND ${refactorTarget}_object_libraries ${OBJ_LIB})

        # Set the target to which the target library belongs
        set(target_container_object_library_${OBJ_LIB} ${refactorTarget})
    endif()
endwhile()

# Remove duplicate targets
list(REMOVE_DUPLICATES TARGETS)


# ===========================================
# Extract files and dependencies from targets

foreach(target ${TARGETS})
    # List of target dependencies
    set(${target}_depen)

    # List of target files
    set(${target}_files)
    

    foreach(object_library ${${target}_object_libraries})
        # Set header file
        set(includeFile "${INCLUDE_DIRECTORY_PATH}/${OBJECT_LIBRARIES_PATH_${object_library}}.${INCLUDE_EXTENSION}")

        # Set source file
        set(srcFile "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${OBJECT_LIBRARIES_PATH_${object_library}}.${SRC_EXTENSION}")

        # Object library content
        set(content)

        if(object_library MATCHES ${MAIN_FILENAME_OBJ_LIB})
            list(APPEND ${target}_files ${srcFile})

            # Read main file content
            file(READ "${srcFile}" content)
        else()
            list(APPEND ${target}_files ${includeFile} ${srcFile})

            # Read header file content
            file(READ "${includeFile}" content)
        endif()

        # Search for all includes in the content
        string(REGEX MATCHALL ${INCLUDE_PATTERN} matches "${content}")

        foreach(match ${matches})
            string(REGEX MATCH ${INCLUDE_PATTERN} matchedPart "${match}")
            set(classPath ${CMAKE_MATCH_1})

            string(REGEX MATCH ${CLASS_NAME} matchedPart "${classPath}")
            set(OBJ_LIB "${CMAKE_MATCH_0}_o")

            list(FIND SIMPLE_OBJECT_LIBRARIES ${OBJ_LIB} index)
            if(NOT index EQUAL -1)
                list(APPEND ${target}_depen ${target_container_object_library_${OBJ_LIB}})
            endif()
        endforeach()

        # Search for all external includes in the content
        string(REGEX MATCHALL ${EXTERNAL_INCLUDE_PATTERN} matches "${content}")
        
        foreach(match ${matches})
            string(REGEX MATCH ${EXTERNAL_INCLUDE_PATTERN} matchedPart "${match}")
            set(class ${CMAKE_MATCH_1})

            list(FIND EXTERNAL_CLASSES ${class} index)
            if(NOT index EQUAL -1)
                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})
                list(APPEND ${target}_depen ${MAP_DEPEN_${refactorClass}})
            endif()
        endforeach()

        # Add dependencies unique to the executable target
        if(target MATCHES ${EXECUTABLE_NAME})
            if(USE_CPU_PROFILER)
                list(APPEND ${target}_depen profiler)
            endif()
            
            if(USE_HEAP_PROFILER)
                list(APPEND ${target}_depen tcmalloc)
            endif()
        endif()
    endforeach()

    # Remove duplicate dependencies
    list(REMOVE_DUPLICATES ${target}_depen)

    # Remove itself from dependencies
    if(NOT target MATCHES ${EXECUTABLE_NAME})
        list(REMOVE_ITEM ${target}_depen ${target})
    endif()
endforeach()
