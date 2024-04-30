
if(NOT BUILD_DOC)
	set(
		BUILD_DOC OFF CACHE STRING
		"Choose the state of documentation build, options are: ON OFF."
		FORCE
	)
endif()

if(BUILD_DOC)
	find_package(
		Doxygen 1.9.1 
		COMPONENTS dot
		OPTIONAL_COMPONENTS mscgen dia
	)

	if(NOT DOXYGEN_FOUND)
		message(WARNING "Doxygen need to be installed to generate the doxygen documentation")
	endif()
endif()

set(CREATED_CLEAN_ALL OFF)

if(DOXYGEN_FOUND AND BUILD_DOC)
	if(RELEASE_MODE)
		set(REMOVE_DOXYGEN_BUILD_SCRIPT "${SCRIPTS_DIRECTORY}/doc/removeDoxygenBuild.py")
		set(GENERATE_SYMBOLIC_LINK_SCRIPT "${SCRIPTS_DIRECTORY}/doc/generateSymbolicLink.py")

		# Set input and output file
		set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
    	set(DOXYGEN_OUT "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")

		# Set documentation build output directory
		set(DOXYGEN_BUILD "doxygen_docs")
	
		# Copy input file into output file and substitutes variable values
		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
		message(STATUS "Doxygen build started")

        add_custom_target(
            clean_doc
            COMMAND ${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${REMOVE_DOXYGEN_BUILD_SCRIPT}" "${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_BUILD}" 
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMENT "Cleaning documentation built output files"
        )

		add_custom_target(
			doc_doxygen ALL
			COMMAND cmake --build --preset ${PRESET_NAME} --target clean_doc &&
					${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} &&
					${Python3_EXECUTABLE} "${PROJECT_SOURCE_DIR}/${GENERATE_SYMBOLIC_LINK_SCRIPT}" "${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_BUILD}/html/index.html" "${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_BUILD}/documentation.html"
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			COMMENT "Generating API documentation with Doxygen"
		)

        add_custom_target(
            clean_all
            COMMAND cmake --build --preset ${PRESET_NAME} --target clean &&
					cmake --build --preset ${PRESET_NAME} --target clean_doc
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )

		set(CREATED_CLEAN_ALL ON)
	else()
		message(WARNING "'Release' need to be set on 'CMAKE_BUILD_TYPE' to generate the doxygen documentation")
	endif()
endif()

if(NOT CREATED_CLEAN_ALL)
	add_custom_target(
		clean_all
		COMMAND cmake --build --preset ${PRESET_NAME} --target clean
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)
endif()