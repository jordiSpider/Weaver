include(cmake/Regex.cmake)

include(cmake/ExternalLibraries.cmake)

# ===========================
# Set libraries consisting of several classes

set(COMPOUND_LIBRARIES
    Animal${UNION}TerrainCellInterface${UNION}TerrainCell
)

# ===========================
# Set libraries consisting of a class

# List all source files

file(GLOB_RECURSE cppFiles ${CPP_FILE_PATTERN})

# List of simple libraries

set(SIMPLE_LIBRARIES)

# Get the class associated with each file

foreach(file ${cppFiles})
    # Get relative file path

    file(RELATIVE_PATH fileRelativePath ${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY} ${file})

    # Remove file path extension

    string(REGEX REPLACE "\\.cpp$" "" fileRelativePathWithoutExtension "${fileRelativePath}")

    # Add class to list of simple libraries

    list(APPEND SIMPLE_LIBRARIES ${fileRelativePathWithoutExtension})
endforeach()

# Remove the classes comprising compound libraries from the simple libraries list

foreach(library ${COMPOUND_LIBRARIES} main)
    string(REPLACE ${UNION} ";" components ${library})

	foreach(class ${components})
        list(FIND SIMPLE_LIBRARIES ${class} index)

        # If the class is not found in the simple libraries, an error message is displayed

        if(index EQUAL -1)
            message(FATAL_ERROR "\"${class}\" class is present in several compound libraries")
        else()
            list(REMOVE_ITEM SIMPLE_LIBRARIES ${class})
        endif()
	endforeach()
endforeach()

# ===========================
# Join all libraries

set(LIBRARIES ${SIMPLE_LIBRARIES} ${COMPOUND_LIBRARIES})

# ===========================
# Get all classes

set(CLASSES)
foreach(library ${LIBRARIES})
	string(REPLACE ${UNION} ";" components ${library})

	foreach(class ${components})
		list(APPEND CLASSES ${class})
	endforeach()
endforeach()

# ===========================
# Map each class to the library it belongs to

foreach(library ${LIBRARIES})
    string(REPLACE "/" ${PATH_SEP} refactorLibrary ${library})

	string(REPLACE ${UNION} ";" components ${refactorLibrary})

	foreach(refactorClass ${components})
        set(MAP_DEPEN_${refactorClass} ${refactorLibrary})
	endforeach()
endforeach()

# ===========================
# Set executable name

if(RELEASE_MODE)
	set(EXECUTABLE_NAME ${PROJECT_NAME})
elseif(DEBUG_MODE)
	set(EXECUTABLE_NAME ${PROJECT_NAME}_debug)
endif()

# ========================================================
# Set main file of the project

set(MAIN_FILENAME "main")

# ===========================
# Set all targets

set(TARGETS ${LIBRARIES} ${EXECUTABLE_NAME})

# ===========================
# Get the components of the targets

foreach(target ${TARGETS})
    string(REPLACE "/" ${PATH_SEP} refactorTarget ${target})

	string(REPLACE ${UNION} ";" components ${target})

    # ===========================
    # List of target files

	set(files)

    # ===========================
    # List of target dependencies

	set(dependencies)

	foreach(filename ${components})
        if(target MATCHES ${EXECUTABLE_NAME})
            # ===========================
            # Set source file

            set(srcFile "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${MAIN_FILENAME}.${SRC_EXTENSION}")

            # ===========================
            # Add header and source files

            list(APPEND files ${srcFile})
        else()
            # ===========================
            # Set header file

            set(includeFile "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${filename}.${INCLUDE_EXTENSION}")

            # ===========================
            # Set source file

            set(srcFile "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${filename}.${SRC_EXTENSION}")

            # ===========================
            # Add header and source files

            list(APPEND files ${includeFile} ${srcFile})
        endif()

        # ===========================
        # Get class dependencies

        set(content)
        if(target MATCHES ${EXECUTABLE_NAME})
            # Read main file content
            file(READ "${srcFile}" content)
        else()
            # Read header file content
            file(READ "${includeFile}" content)
        endif()

        # List of class dependencies

        set(dependenciesClass)

        # Search for all includes in the content

        string(REGEX MATCHALL ${INCLUDE_PATTERN} matches "${content}")
        
        foreach(match ${matches})
            string(REGEX REPLACE ${INCLUDE_PATTERN} "\\1" class "${match}")
            
            list(FIND CLASSES ${class} index)
            if(NOT index EQUAL -1)
                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})
                list(APPEND dependenciesClass ${refactorClass})
            endif()
        endforeach()


        string(REGEX MATCHALL ${EXTERNAL_INCLUDE_PATTERN} matches "${content}")
        
        foreach(match ${matches})
            string(REGEX REPLACE ${EXTERNAL_INCLUDE_PATTERN} "\\1" class "${match}")
            
            list(FIND EXTERNAL_CLASSES ${class} index)
            if(NOT index EQUAL -1)
                string(REPLACE "/" ${PATH_SEP} refactorClass ${class})
                list(APPEND dependenciesClass ${refactorClass})
            endif()
        endforeach()

        # ===========================
        # Add class dependencies

		list(APPEND dependencies ${dependenciesClass})
    endforeach()

    # ===========================
    # Replace the classes with the library that contains them

	set(mappedDependencies)
	foreach(dependency ${dependencies})
		list(APPEND mappedDependencies ${MAP_DEPEN_${dependency}})
	endforeach()

    # ===========================
    # Remove duplicates libraries

	list(REMOVE_DUPLICATES mappedDependencies)

    # ===========================
    # Remove itself from dependencies

    if(NOT target MATCHES ${EXECUTABLE_NAME})
        list(REMOVE_ITEM mappedDependencies ${refactorTarget})
    else()
        if(USE_CPU_PROFILER)
            list(APPEND mappedDependencies profiler)
        endif()
        
        if(USE_HEAP_PROFILER)
            list(APPEND mappedDependencies tcmalloc)
        endif()
    endif()

    # ===========================
    # Set final library dependencies

	set(dependencies ${mappedDependencies})

    # ===========================
    # Set library files

	set(${refactorTarget}_files ${files})

    # ===========================
    # Set library dependencies

	set(${refactorTarget}_depen ${dependencies})
endforeach()