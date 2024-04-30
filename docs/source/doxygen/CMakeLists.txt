
if(BUILD_SPHINX_DOC OR BUILD_DOXYGEN_DOC OR BUILD_DOXYGEN_CONF)
	# Set source code documentation build output directory
	set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

	# Set input and output doxyfile
	set(DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
	set(DOXYFILE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")

	# Copy input file into output file and substitutes variable values
	configure_file(${DOXYFILE_TEMPLATE} ${DOXYFILE} @ONLY)

	if(BUILD_SPHINX_DOC OR BUILD_DOXYGEN_DOC)
		set(DOXYGEN_VERSION 1.9.1)

		find_package(
			Doxygen ${DOXYGEN_VERSION} 
			COMPONENTS dot
			OPTIONAL_COMPONENTS mscgen dia
		)

		if(NOT DOXYGEN_FOUND)
			message(FATAL_ERROR "Doxygen (v${DOXYGEN_VERSION}+) need to be installed to generate the documentation")
		endif()

		add_custom_target(
			build_doxygen_doc ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Generating source code documentation with Doxygen"
		)
	endif()
endif()
