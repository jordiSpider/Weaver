
# ===========================
# Create libraries of all classes

foreach(class ${CLASSES_NAMES_CMAKE_PATH})

    # ===========================
    # Generate refactor name of the class
    #
    # This refactor name qualifies as a target name
    # Example: Maths/Random --> Maths_Random   

    execute_process(
        COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${REFACTOR_CLASS_SCRIPT}" "${class}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
        OUTPUT_VARIABLE refactorClass
    )
    
    # ===========================
    # Generate library of the class

    add_library(
        ${refactorClass} STATIC
        ${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${class}.${INCLUDE_EXTENSION}
        ${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${class}.${SRC_EXTENSION}
    )

    # ===========================
    # Set include directories for library

    target_include_directories(${refactorClass} PUBLIC "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/")

    # ===========================
    # Set compile flags of library

    target_compile_options(${refactorClass} PRIVATE ${CXX_FLAGS})

endforeach()

# ===========================
# Link all libraries

foreach(class ${CLASSES_NAMES_CMAKE_PATH})

    # ===========================
    # Generate refactor name of the class
    #
    # This refactor name qualifies as a target name
    # Example: Maths/Random --> Maths_Random

    execute_process(
        COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${REFACTOR_CLASS_SCRIPT}" "${class}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
        OUTPUT_VARIABLE refactorClass
    )

    # ===========================
    # List link libraries of library's source code file

    execute_process(
        COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${LIST_LINK_LIBRARIES_SCRIPT}" "${PROJECT_SOURCE_DIR}/${SRC_DIRECTORY}/${class}.${SRC_EXTENSION}" "${class}" "${CLASSES_NAMES_CMAKE_PATH}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
        OUTPUT_VARIABLE LINK_LIBRARIES_SRC
    )

    # ===========================
    # List link libraries of library's header file

    execute_process(
        COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${LIST_LINK_LIBRARIES_SCRIPT}" "${PROJECT_SOURCE_DIR}/${INCLUDE_DIRECTORY}/${class}.${INCLUDE_EXTENSION}" "${class}" "${CLASSES_NAMES_CMAKE_PATH}"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
        OUTPUT_VARIABLE LINK_LIBRARIES_INCLUDE
    )

    # ===========================
    # Merge the two list of link libraries

    set(LINK_LIBRARIES ${LINK_LIBRARIES_SRC})
    list(APPEND LINK_LIBRARIES ${LINK_LIBRARIES_INCLUDE})

    # ===========================
    # Link libraries with library of the class    

    target_link_libraries(
    	${refactorClass}
    	PRIVATE
    	${LINK_LIBRARIES}
    )

endforeach()
